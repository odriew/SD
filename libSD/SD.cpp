/*
 *  SD.cpp
 *  
 *
 *  Created by Daniel Dobrosky on 8/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <libSD/SD.h>

//slow baud for init. fast for opp.
extern const spi_baud_rate baudSlow= SPI_BAUD_PCLK_DIV_256;
extern const spi_baud_rate baudFull= SPI_BAUD_PCLK_DIV_2;

SD_Dev::SD_Dev(){}

SD_Dev::~SD_Dev(){
	
}

unsigned char SD_Dev::init(spi_dev *_spi, gpio_dev *_chSel, spi_mode _mode, spi_cfg_flag _flags){	
  this->spi=_spi; this->chSel= _chSel; this->mode= _mode; this->flags= _flags;	//required devices
  dma= 0;
  spi_init(spi);


  baud= baudSlow;
	
	SerialUSB.println("2.1");
	
       	gpio_init(chSel);	//CS initialization
	spi_gpio_cfg(1, chSel, cspin, chSel, 5, 6, 7);
	
	spi_master_enable(spi, baud, mode, flags);
	if(spi_is_enabled(spi)) SerialUSB.println("yep");
	else SerialUSB.println("dammit");
//	TODO: SD init. sequence #DONE#
	int i;
	cptr= 0;
	SerialUSB.println("2.2");
	//--sync process
	spi_activate(chSel, cspin);
	for(i= 0; i < 100; i++){
		spi_send(spi, 0xFF);
	}
	SerialUSB.println("2.3");	
	spi_deactivate(chSel, cspin);
	for(i=0; i < 5; i++) spi_send(spi, 0xFF);
	for(i= 0; i < 4; i++) sdArgs[i]= 0x00;
	
	do if(command(CMD0, sdArgs, sdResponse, CMD0R)){SerialUSB.println("Aww"); while(1);}
	while((sdResponse[0] & 0xFE));
	
	SerialUSB.println(sdResponse[0], DEC);	
	SerialUSB.println("2.4");

	sdArgs[0]= 0b10101010;	//check pattern
	sdArgs[1]= 0x01 & 0x0F;	//acceptable voltage range
	sdArgs[2]= 0x00;
	sdArgs[3]= 0x00;
	do{
	  command(CMD8, sdArgs, sdResponse, CMD8R);
	  SerialUSB.println(sdResponse[0]);
	}
	while(sdResponse[0] & 0xFE);
	
	SerialUSB.println("2.5");

	sdArgs[0]= 0x00;
	sdArgs[1]= 0x00;
	sdArgs[2]= 0x00;
	sdArgs[3]= 0x40;
	i= 0;
	do{
	  if(!command(CMD55, sdArgs, sdResponse, CMD55R)){
	    SerialUSB.print("a");SerialUSB.println(sdResponse[0]);
	    command(ACMD41, sdArgs, sdResponse, ACMD41R);
	    SerialUSB.print("b");SerialUSB.println(sdResponse[0]);
	  }
	  else return 1;
	}
	while((sdResponse[0] & 0x01) && (++i < 1000));
	if(i >= 1000) return 1;
	
	SerialUSB.println("2.6");

	do {
	  if(command(CMD58, sdArgs, sdResponse, CMD58R)) {
	    SerialUSB.println("DAMN");
	    return 1;
	  }
	  //	  SerialUSB.println("-");
      	  //SerialUSB.println(sdResponse[2], DEC);
	}
	while((sdResponse[2] & 0xC0) != 0xC0);
	
	SerialUSB.println("2.7");
		
	baud= baudFull;
	spi_master_enable(spi, baud, mode, flags);
	
	SerialUSB.println("2.8");

	return 0;
}

int SD_Dev::command(int cmd, unsigned char *arg, unsigned char *resp, int respType){
	unsigned char i= 0;
	unsigned char respLeng= 0;
	unsigned char charQ;
	spi_activate(chSel, cspin);
	for(i=0; i<10; i++)spi_send(spi, 0xFF);
	spi_send(spi, (cmd & 0x3F) | 0x40);
	for(i= 0; i < 4; i++) {
		spi_send(spi, arg[3 - i]);
	}
	
	if (cmd == CMD0) spi_send(spi, 0x95);		//CRC (0x95) for CMD0
	else if(cmd == CMD8) spi_send(spi, 0x87);	//CRC (0x87) for CMD8
	else spi_send(spi, 0xFF);					//CRC otherwise goes unchecked
	switch(respType){
		case R1:
		case R1Busy:
			respLeng= 1;
			break;
		case R2:
			respLeng= 2;
			break;
		case R3:
		case R7:
			respLeng= 5;
			break;
		default:
			break;
	}
	
	i= 0;
	do{
		charQ= spi_send(spi, 0xFF);
		i++;
	}
	while(((charQ & 0x80) != 0) && (i < 10000));
	if(i >= 10000){
		spi_deactivate(chSel, cspin);
		return 1;
	}
	for(i= 0; i < respLeng; i++){
		resp[i]= charQ;
		charQ= spi_send(spi, 0xFF);
	}
	if(respType == R1Busy){
		do charQ= spi_send(spi, 0xFF);
		while(charQ != 0xFF);
		spi_send(spi, 0xFF);
	}
	delay(100);
	spi_deactivate(chSel, cspin);
	return 0;
//##################################################################
}

int SD_Dev::readBlock(unsigned long int blockAdd, char *dest){
  long int i;
  char aChar;
  spi_activate(chSel, cspin);
  for(i= 0; i < 100; i++) spi_send(spi, 0xff);

  //  while(spi_send(spi, 0xff) != 0xFF);
  do{
    aChar= spi_send(spi, 0xff);
    SerialUSB.println("^^^^^^^^");
    SerialUSB.println(aChar, DEC);
    //  delay(100);
  }
  while(aChar != 0xff);
  blockAdd *= 512;
  for(i= 3; i >= 0; i--) sdArgs[i]= (unsigned char)((blockAdd >> (8 * i)) & 0xff);
    //single block read command
  if(command(CMD17, sdArgs, sdResponse, CMD17R)) { spi_deactivate(chSel, cspin);SerialUSB.println("dammit"); return 1;}
  SerialUSB.println("???????????");
  SerialUSB.println(sdResponse[0], BIN);  
  if(sdResponse[0]){SerialUSB.println("aaaaaaaaaaaaaaaaaaaaaaaaa"); return 1;}
  delay(100);
  i= 0;
  spi_activate(chSel, cspin);
  do{
    aChar= spi_send(spi,0xff);
    i++;
    SerialUSB.println("*********");
    SerialUSB.println(aChar, DEC);
  }
  while((aChar == 0xff) && (i < 40000));
  SerialUSB.println("$$$$$$$$$$$");
  //  SerialUSB.println(aChar, DEC);

  if(i >= 40000){
    spi_deactivate(chSel, cspin);
    SerialUSB.println("balls");
    return 1;
  }
  
  if((aChar & 0xe0) == 0){
    spi_send(spi, 0xff);
    spi_deactivate(chSel, cspin);
    SerialUSB.println("!!!!!!!!!!!!");
    SerialUSB.println(aChar, DEC);
    return 1;
  }

  if(aChar != 0xFE){
    spi_deactivate(chSel, cspin);
    SerialUSB.println("nooooooooooooooooooooo");
    SerialUSB.println(aChar, DEC);
    return 1;
  }
  for(i= 0; i < 512; i++){
    dest[i]= spi_send(spi, 0xff);
  }
  spi_send(spi, 0xff);
  spi_send(spi, 0xff);
  spi_deactivate(chSel, cspin);
  return 0;
}

int SD_Dev::writeBlock(unsigned long int blockAdd, char* src){
  spi_activate(chSel, cspin);
  while(spi_send(spi, 0xff) != 0xff);

  int i;
  char q;
  blockAdd *= 512;
  for(i= 3; i >= 0; i--) sdArgs[i]= (unsigned char)((blockAdd >> (8 * i)) & 0xff);
  do if(command(CMD24, sdArgs, sdResponse, CMD24R)){ spi_deactivate(chSel, cspin); return 1;}
  while(sdResponse[0]);

  spi_send(spi, 0xff);
  spi_send(spi, 0xfe);
  for(i= 0; i < 512; i++) spi_send(spi, src[i]);
  spi_send(spi, 0xff);
  spi_send(spi, 0xff);

  do q= spi_send(spi, 0xff);
  while((q & 0x10) || !(q & 0x01));

  if((q & 0x1F) != 0x05) {spi_deactivate(chSel, cspin); return 1;}
  spi_deactivate(chSel, cspin);
  return 0;
}

//requisite spi functions
unsigned char spi_send(spi_dev *spi, unsigned char data){
	unsigned char txBuf[1];
	txBuf[0]= data;
	spi_tx(spi, txBuf, 1);
       	while(!spi_is_rx_nonempty(spi));
	return spi_rx_reg(spi);
}
