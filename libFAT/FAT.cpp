/*
 *  FAT.cpp
 *  
 *
 *  Created by Daniel Dobrosky on 8/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <libFAT/FAT.h>
//TODO: FAT init. that first initializes SD, then the FAT pointer map

Partition::Partition(){
}

Partition::~Partition(){
}

int Partition::init(SD_Dev* _sd){
  sd= _sd;
  SerialUSB.println("1.1");
  SerialUSB.println("1.2");
  int i;
  unsigned long int fatOff= 0, fatLeng= 0;
  if(sd->readBlock(0, sd->blockBuf)) {SerialUSB.println("naw");return 1;}
  if(!sd->blockBuf[0]) baseAdd= sd->blockBuf[454];
  else baseAdd= sd->blockBuf[0];
  //  SerialUSB.println(baseAdd);

  if(sd->readBlock(baseAdd, sd->blockBuf)) {SerialUSB.println("nope");return 1;}
  SerialUSB.println("yes?");
  for(i= 0; i < 2; i++) fatOff += (sd->blockBuf[14 + i] << (8 * i));
  for(i= 0; i < 4; i++) fatLeng += (sd->blockBuf[36 + i] << (8 * i));

  spc= sd->blockBuf[13];
  SerialUSB.println("~~~~~~~~");
  SerialUSB.println(spc, DEC);
  fatAdd= fatOff + baseAdd;
  rootAdd= (2 * fatLeng) + fatOff + baseAdd;
  //  SerialUSB.println(rootAdd, DEC);
  return 0;
}

FileBox::FileBox(Partition *_partition){
  partition= _partition;
}

FileBox::~FileBox(){
}

unsigned long int FileBox::getNextCluster(unsigned long int thisCluster){
  unsigned long int blockQ, returnQ= 0;
  unsigned int offset;
  blockQ= partition->fatAdd;
  blockQ += (thisCluster / 128);
  if(blockQ != partition->sd->cptr){
    if(!partition->sd->readBlock(blockQ, partition->sd->blockBuf)) return 1;
    partition->sd->cptr= blockQ;
  }
  offset= (thisCluster % 128) * 4;
  for(int i= 0; i < 4; i++) returnQ += (partition->sd->blockBuf[offset + i] << (8 * i));
  return returnQ;
}

void  FileBox::getClusterChain(){
  unsigned long int *chain, link, length= 1;

  chain= (unsigned long int*)malloc(sizeof(unsigned long int) * length);
  chain[0]= 0x0FFFFFFF;
  SerialUSB.println(startCluster, DEC); while(3.14);
  link= getNextCluster(startCluster);
  while(link != 0x0FFFFFFF){
    chain[length - 1]= link;
    length++;
    chain= (unsigned long int*)realloc(chain, sizeof(unsigned long int) * length);
    link= getNextCluster(link);
  }

  chainLength= length;
  clusterChain= chain;
}

void FileBox::setClusterChain(){
  unsigned long int p= 0, blockQ= partition->fatAdd + (startCluster / 128), offset= (startCluster % 128) * 4;
  SD_Dev* sd= partition->sd;
 
 if(sd->readBlock(blockQ, sd->blockBuf)) return;
 partition->cacheQ= blockQ;

  do{
    if(blockQ != partition->cacheQ){
      if(sd->writeBlock(partition->cacheQ, sd->blockBuf)) return;
      if(sd->readBlock(blockQ, sd->blockBuf)) return;
      partition->cacheQ= blockQ;
    }

    for(int i= 0; i < 4; i++)
      sd->blockBuf[offset + i]= (char)((clusterChain[p] >> (8 * i)) & 0xFF);
    blockQ= partition->fatAdd + (clusterChain[p] % 128) * 4;
  }
while(++p < chainLength);
}

void FileBox::addLink(){
  unsigned long int freeQ= 0;
  unsigned long int clusterQ= clusterChain[chainLength - 1] + 4;
  freeQ= getNextCluster(clusterQ);
  freeQ &= 0x0FFFFFFF;

  while(freeQ != 0){}
}
