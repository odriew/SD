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

SD_Dev::SD_Dev(spi_dev *_spi, gpio_dev *_chSel){
	spi=_spi; chSel= _chSel;	//required devices
	dma= 0;
	spi_init(spi);
}

SD_Dev::SD_Dev(spi_dev *_spi, gpio_dev *_chSel, dma_dev *_dma){
	spi=_spi; chSel= _chSel; dma= _dma;	//required devices w/ DMA
	spi_init(spi);
	dma_init(dma);
}

SD_Dev::~SD_Dev(){
	
}

unsigned char SD_Dev::init(spi_mode _mode, spi_cfg_flag _flags){	
	mode= _mode;
	baud= baudSlow;
	flags= _flags;
	
	gpio_init(chSel);	//CS initialization
	gpio_set_mode(chSel, cspin, GPIO_OUTPUT_PP);
	
	spi_master_enable(spi, baud, mode, flags);
//	TODO: SD init. sequence #DONE#
	int i;
	
	cptr= 0;
		
	//--sync process
	for(i= 0; i < 100; i++){
		spi_send(spi, 0xFF);
	}
	
	for(i= 0; i < 4; i++) sdArgs[i]= 0x00;
	
	do command(CMD0, sdArgs, sdResponse, CMD0R);
	while(sdResponse[0] & 0xFE);
	
	sdArgs[0]= 0b10101010;	//check pattern
	sdArgs[1]= 0x01 & 0x0F;	//acceptable voltage range
	sdArgs[2]= 0x00;
	sdArgs[3]= 0x00;
	do command(CMD8, sdArgs, sdResponse, CMD8R);
	while(sdResponse[0] & 0xFE);
	
	sdArgs[0]= 0x00;
	sdArgs[1]= 0x00;
	sdArgs[2]= 0x00;
	sdArgs[3]= 0x40;
	i= 0;
	do{
		if(!command(CMD55, sdArgs, sdResponse, CMD55R))
			command(ACMD41, sdArgs, sdResponse, ACMD41R);
		else return 1;
	}
	while((sdResponse[0] & 0x01) && (++i < 1000));
	if(i >= 1000) return 1;
	
	do {if(!command(CMD58, sdArgs, sdResponse, CMD58R)) return 1;}
	while(!(sdResponse[1] & 0x80));
	
	baud= baudFull;
	spi_master_enable(spi, baud, mode, flags);
	return 0;
	
}

int SD_Dev::command(int cmd, unsigned char *arg, unsigned char *resp, int respType){
	unsigned char i= 0;
	unsigned char respLeng= 0;
	unsigned char charQ;
	unsigned char txbuf[1];
//#############TODO: convert section to utilize libmaple for spi #DONE#	
	spi_activate(chSel, cspin);
	spi_send(spi, 0xFF);
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
	spi_deactivate(chSel, cspin);
	return 0;
//##################################################################
}

int SD_Dev::readBlock(unsigned long int blockAdd, char *dest){
	return 0;
}

int SD_Dev::writeBlock(unsigned long int blockAdd, char* src){
	return 0;
}

//requisite spi functions
unsigned char spi_send(spi_dev *spi, unsigned char data){
	unsigned char txBuf[1];
	txBuf[0]= data;
	spi_tx(spi, txBuf, 1);
	while(spi_is_rx_nonempty(spi));
	return spi_rx_reg(spi);
}
