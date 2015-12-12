#include "room.h"

typedef Room* room;

__thread room selr;
static room first,last;

void roominit(){
    first=new Room; last=new Room;
    first->next=last; last->prev=first;
}

void roomuninit(){
    delete first; delete last;
}

void roomsel(char*name){
    printf("selecting room %s\n",name);
    room r;
    for(r=first->next;r!=last;r=r->next){
        if(!strcmp(r->name,name)){
            selr=r;
            return;
        }
    }
    printf("creating new room %s\n",name);
    r=new Room;
    r->name=new char[strlen(name)+1]; strcpy(r->name,name);
    r->prev=last->prev; last->prev->next=r;
    r->next=last; last->prev=r;
    r->nc=0;
    selr=r;
}

int roomadd(){
    int i;
    for(i=0;i<selr->nc;++i) if(selc==selr->clients[i]) return INROOM;
    if(selr->nc==ROOMSIZE) return ROOMFULL;
    selr->clients[selr->nc++]=selc; return JOINOK;
}
