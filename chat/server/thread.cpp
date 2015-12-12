#include "thread.h"
#include "room.h"
#include "network.h"
static client first,last;
static int lastid;
void threadInit(){
    first=new Client;
    last=new Client;
    first->next=last;
    last->prev=first;
}
static char selectuser(char*name){
    client c;
    for(c=first->next;c!=last;c=c->next){
        if(!c->loggedin)continue;
        if(!strcmp(c->name,name)){selc=c;return 1;}
    }
    return 0;
}
static char nameinuse(char*name){
    client c;
    for(c=first->next;c!=last;c=c->next){
        if(!c->loggedin)continue;
        if(!strcmp(c->name,name))return 1;
    }
    return 0;
}
static void threadRun(client c){
    while(1){
        int msgid=recvi();
        switch(msgid){
            case MSGNAME:{
                if(c->loggedin) throw "already logged in";
                char*name=recvs(NAMEMIN,NAMEMAX);
                if(nameinuse(name)){
                    printf("%s: name already in use\n",name);
                    delete[]name;
                    senderr(NAMEINUSE); break;
                }
                strcpy(c->name,name); delete[]name;
                c->loggedin=1;
                printf("Client %i logged in with name %s\n",c->id,c->name);
                senderr(NAMEOK);
                break;
            }
            case MSGALL:{
                if(!c->loggedin) throw "not logged in";
                char*msg=recvs(MSGMIN,MSGMAX);
                for(selc=first->next;selc!=last;selc=selc->next){
                    sendi(MSGFROM); sends(c->name); sends(msg);
                }
                selc=c;
                delete[]msg;
                break;
            }
            case MSGTO:{
                if(!c->loggedin) throw "not logged in";
                char*name=recvs(NAMEMIN,NAMEMAX);
                char* msg=recvs(MSGMIN,MSGMAX);
                if(!selectuser(name)){
                    senderr(NOSUCHUSER);
                    delete[]name; delete[]msg;
                    break;
                }
                sendi(MSGFROM); sends(c->name); sends(msg);
                delete[]name; delete[]msg;
                selc=c;
                break;
            }
            case MSGROOM:{
                if(!c->loggedin) throw "not logged in";
                char*room=recvs(ROOMMIN,ROOMMAX);
                char* msg=recvs(MSGMIN,MSGMAX);
                roomsel(room);
                if(!inroom()){
                    delete[]room; delete[]msg;
                    senderr(NOTINROOM); break;
                }
                roommsg(msg);
                delete[]room;delete[]msg;
                break;
            }
            case MSGJOIN:{
                if(!c->loggedin) throw "not logged in";
                char*room=recvs(ROOMMIN,ROOMMAX);
                roomsel(room); senderr(roomadd());
                break;
            }
            default:{
                printf("Msg id: %i\n",msgid);
                throw "unknown msg id";
                break;
            }
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
    delete (SOCKET*)socketData;
    last->prev->next=c;
    c->prev=last->prev;
    c->next=last;
    c->id=++lastid;
    c->loggedin=0;
    last->prev=c;
    selc=c;
    printf("Client %i has joined\n",c->id);
    try{
        threadRun(c);
    }
    catch(const char*msg){
        printf("Error at client %i : %s\n",c->id,msg);
    }
    threadUninit(c);
    return 0;
}

