/*
 *  main.cpp
 *  
 *
 *  Created by Daniel Dobrosky on 8/7/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <libSD/SD.h>
#include <libFAT/FAT.h>

spi_dev *spi= SPI1;
gpio_dev *chSel= GPIOA;
dma_dev *dma= DMA1;
SD_Dev sd (spi, chSel, dma);

extern const spi_mode mode= SPI_MODE_0;
extern const spi_cfg_flag flags= SPI_FRAME_MSB;

int main(void){	
	//initialize slow spi for SD initialization
	sd.init(mode, flags);
}