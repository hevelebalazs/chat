#include "thread.h"
static client first,last;
static int buffsize=BUFFSIZE;
static int lastId;

void threadInit(){
    first=new Client;
    last=new Client;
    first->next=last;
    last->prev=first;
}
static void threadRun(client c){
    while(1){
        int res;
        res=recv(c->sock,c->buff,buffsize,0);
        if(res==0||res==SOCKET_ERROR){
            //client was disconnected
            printf("Client %i has left\n",c->id);
            break;
        }
        char msg[1024];
        sprintf(msg,"Client %i: %s\n",c->id,c->buff);
        printf(msg);
        //send data to everyone
        for(client to=first->next;to!=last;to=to->next){
            res=send(to->sock,msg,buffsize,0);
            if(res==SOCKET_ERROR) throw "send error!";
        }
    }
}
static void threadUninit(client c){
    if(c->sock)closesocket(c->sock);
    c->next->prev=c->prev;
    c->prev->next=c->next;
    delete c;
}
DWORD WINAPI threadNew(void*socketData){
    client c=new Client;
    c->sock=*(SOCKET*)socketData;
    last->prev->next=c;
    c->prev=last->prev;
    c->next=last;
    c->id=++lastId;
    printf("Client %i has joined\n",c->id);
    last->prev=c;
    try{
        threadRun(c);
    }
    catch(const char*msg){
        printf("Error at client %i : %s\n",c->id,msg);
    }
    threadUninit(c);
    return 0;
}

