#include "thread.h"
#include "room.h"
#include "network.h"
static client first,last;
static int lastid;
static CRITICAL_SECTION critsec;
void threadInit(){
    first=new Client;
    last=new Client;
    first->next=last;
    last->prev=first;
    InitializeCriticalSection(&critsec);
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
        EnterCriticalSection(&critsec);
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
            case MSGLEAVE:{
                if(!c->loggedin) throw "not logged in";
                char*room=recvs(ROOMMIN,ROOMMAX);
                roomsel(room);
                if(!inroom()) senderr(NOTINROOM);
                roomrm();
                break;
            }
            case MSGFILE:{
                printf("%s wants to send file:\n",c->name);
                if(!c->loggedin) throw "not logged in";
                char*name=recvs(NAMEMIN,NAMEMAX);
                printf("  to %s\n",name);
                if(!selectuser(name)) throw "User not found for file sending";
                client to=selc; selc=c;
                char*fname=recvs(FNAMEMIN,FNAMEMAX);
                printf("  file name %s\n",fname);
                int   flen=recvi();
                printf("  file size %i\n",flen);
                printf("  data:\n");
                if(flen<FILEMIN) throw "File too short!";
                if(flen>FILEMAX) throw "File too long!";
                selc=to; sendi(MSGFILE); sends(c->name); sends(fname); sendi(flen);
                char buff[FILEBUFF];
                int sent=0;
                while(sent<flen){
                    selc=c;
                    int read=FILEBUFF;
                    if(sent+read>flen) read=flen-sent;
                    recvn(buff,read);
                    selc=to; sendn(buff,read);
                    sent+=read;
                }
                selc=c;
                printf("Finished reading\n");
                delete[]name; delete[]fname;
                break;
            }
            default:{
                printf("Msg id: %i\n",msgid);
                throw "unknown msg id";
                break;
            }
        }
        LeaveCriticalSection(&critsec);
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
        LeaveCriticalSection(&critsec);
    }
    threadUninit(c);
    return 0;
}
