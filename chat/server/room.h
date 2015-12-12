#ifndef ROOMH
#define ROOMH
#include "thread.h"
#include "network.h"

typedef struct ROOM{
    struct ROOM* prev,* next;
    char* name;
    int nc; //number of clients
    client clients[ROOMSIZE];
}Room;

extern __thread client selc; //selected client
extern __thread Room* selr; //selected room

void roominit();
void roomuninit();

char inroom();
void roomsel(char*);
int  roomadd();
int  roomrm();
void roommsg(char*);
#endif
