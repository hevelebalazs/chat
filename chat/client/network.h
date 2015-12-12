#ifndef NETWORKH
#define NETWORKH
#include <winsock2.h>
#include <stdio.h>
#include "constants.h"

 void init();
 void uninit();

 void recvn(char*,int);
  int recvi();
char* recvs(int,int);

 void sendn(char*,int);
 void sendi(int);
 void sends(char*);

#endif
