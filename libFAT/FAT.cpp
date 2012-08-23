/*
 *  FAT.cpp
 *  
 *
 *  Created by Daniel Dobrosky on 8/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <libFAT/FAT.h>
#include <FATprivate.h>
//TODO: FAT init. that first initializes SD, then the FAT pointer map

int Partition::init(SD_Dev sd){
  sd.readBlock();
}
