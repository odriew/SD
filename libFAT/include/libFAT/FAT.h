/*
 *  FAT.h
 *  
 *
 *  Created by Daniel Dobrosky on 8/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#include <stdlib.h>
#include <FATprivate.h>

class Partition{
  unsigned long int baseAdd;
  unsigned long int fatAdd;
  unsigned long int rootAdd;
  int spc;

public:
  Partition();
  ~Partition();
  int init(SD_Dev sd);
};

