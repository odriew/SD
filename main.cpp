/*
 *  main.cpp
 *  
 *
 *  Created by Daniel Dobrosky on 8/7/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <libFAT/FAT.h>
#include <libmaple/usart.h>

//fuck it
//#include <wirish/wirish.h>

spi_dev *spi= SPI1;
gpio_dev *chSel= GPIOA;

extern const spi_mode mode= SPI_MODE_0;
extern const spi_cfg_flag flags= SPI_FRAME_MSB;

SD_Dev sd;

Partition partition; //NOTE: partition initializes sd via address
FileBox test(&partition);

void setup(){
  while(sd.init(spi, chSel, mode, flags));
   while(!SerialUSB.available())
    continue;
  SerialUSB.read();
  SerialUSB.println("Initializing...");
  partition.init(&sd);
  SerialUSB.println("-------------------------------DONE");
  
}

void loop(){
  // SerialUSB.println(partition.fatAdd);
  //SerialUSB.println("~~~~~~~~~~");
  //SerialUSB.println();
  
   sd.readBlock(partition.rootAdd, sd.blockBuf);
  for(int q=0; q <=9; q++){
    for(int i=q*30; i < (q*30 + 30); i++){
      SerialUSB.print(i); SerialUSB.print(": 0x");SerialUSB.println(sd.blockBuf[i], HEX);
    }
    while(!SerialUSB.available());
    SerialUSB.read();
  }
}


__attribute__((constructor)) void premain(){
  init();
}

int main(void){	
  setup();
  while(1)
    loop();
}
