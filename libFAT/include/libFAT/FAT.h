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
 public:
  unsigned long int baseAdd;
  unsigned long int rootAdd;
  int spc;

 public:
 SD_Dev *sd;
 unsigned long int fatAdd;
 unsigned long int cacheQ;

  Partition();
  ~Partition();
   int init(SD_Dev *_sd);
};

class FileBox{
  Partition *partition;

  char filename[13];
  unsigned long int startCluster;
  unsigned long int *clusterChain;
  unsigned int chainLength;
  unsigned long int getNextCluster(unsigned long int thisCluster);
 public:
  FileBox(Partition *_partition);
  ~FileBox();
  void getClusterChain();
  void setClusterChain();
  void addLink();
};
