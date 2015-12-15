#include "room.h"

typedef Room* room;

__thread room selr;
static room first,last;

void roominit(){
    first=new Room; last=new Room;
    first->next=last; last->prev=first;
}
char inroom(){
    int i;
    for(i=0;i<selr->nc;++i) if(selc==i[selr->clients]) return 1;
    return 0;
}
void roomuninit(){
    delete first; delete last;
}
void roomsel(char*name){
    room r;
    for(r=first->next;r!=last;r=r->next){
        if(!strcmp(r->name,name)){
            selr=r;
            return;
        }
    }
    r=new Room;
    r->name=new char[strlen(name)+1]; strcpy(r->name,name);
    r->prev=last->prev; last->prev->next=r;
    r->next=last; last->prev=r;
    r->nc=0;
    selr=r;
}
void roommsg(char*msg){
    client c=selc;
    int i, n=selr->nc;
    for(i=0;i<n;++i){
        selc=i[selr->clients];
        sendi(MSGROOM); sends(selr->name); sends(c->name); sends(msg);
    }
    selc=c;
}
int roomadd(){
    if(inroom())return INROOM;
    if(selr->nc==ROOMSIZE) return ROOMFULL;
    selr->clients[selr->nc++]=selc;
    int i, n=selr->nc;
    client c=selc;
    for(i=0;i<n;++i){
        selc=i[selr->clients];
        sendi(MSGJOIN); sends(selr->name); sends(c->name);
    }
    selc=c;
    return JOINOK;
}
static void roomdel(){
    selr->prev->next=selr->next;
    selr->next->prev=selr->prev;
    delete[]selr->name; delete selr;
}
void roomrm(){
    int i,n=selr->nc; client*C=selr->clients;
    for(i=0;i<n;++i){
        if(selc==i[selr->clients])break;
    }
    n=--selr->nc;
    if(n==0)roomdel();
    i[C]=n[C];
    client c=selc;
    for(i=0;i<n;++i){
        selc=i[C];
        sendi(MSGLEAVE); sends(selr->name); sends(c->name);
    }
    selc=c;
}
