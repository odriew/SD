/*
 *  SD.h
 *  
 *
 *  Created by Daniel Dobrosky on 8/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <libmaple/spi.h>
#include <libmaple/dma.h>
#include <libmaple/gpio.h>

#include <wirish/wirish.h>

#define R1		1
#define R1Busy	2
#define R2		3
#define R3		4
#define R7		5

//initialize command
#define CMD0 	0
#define CMD0R	R1
//send valid voltage range
#define CMD8	8
#define CMD8R	R7
//next command app-specific
#define CMD55	55
#define CMD55R	R1
//check some condition
#define ACMD41	41
#define ACMD41R	R1
//read single block
#define CMD17	17
#define CMD17R	R1
//write single block
#define CMD24	24
#define CMD24R	R1
//read OCR
#define CMD58	58
#define CMD58R	R3

#define cspin 10

//NOTE: spi_activate() and spi_deactivate() to be used only after GPIOA is initialized
#define spi_activate(chSel, cspin)		gpio_write_bit(chSel, cspin, 0)
#define spi_deactivate(chSel, cspin)	gpio_write_bit(chSel, cspin, 1);

class SD_Dev {
//Standard variables
	spi_dev *spi;
	spi_mode mode;
	spi_baud_rate baud;
	spi_cfg_flag flags;
	
//GPIO variables for CS pin opperation
	gpio_dev *chSel;
	
//Optional DMA variables
	dma_dev *dma;
	static const dma_channel channel_rx= DMA_CH2;
	static const dma_channel channel_tx= DMA_CH3;
	
public:
//Cache pointer (cptr), points to currently loaded block on SD card
	uint32 cptr;
	
//SD command argument and response arrays
	unsigned char sdArgs[4];
	unsigned char sdResponse[5];

	//Buffer array
	char blockBuf[512];

	SD_Dev();
	~SD_Dev();
	unsigned char init(spi_dev *_spi, gpio_dev *_chSel, spi_mode _mode, spi_cfg_flag _flags);
	int command(int cmd, unsigned char *arg, unsigned char *resp, int respType);
	int readBlock(unsigned long int blockAdd, char *dest);
	int writeBlock(unsigned long int blockAdd, char* src);
};

unsigned char spi_send(spi_dev *spi, unsigned char data);
