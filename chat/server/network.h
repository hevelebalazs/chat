#ifndef NETWORKH
#define NETWORKH

#include "../client/constants.h"
#include "thread.h"

extern __thread client selc;

 void recvn(char*,int);
  int recvi();
char* recvs(int,int);

 void sendn(char*,int);
 void sendi(int);
 void sends(char*);
 void senderr(int);

#endif
