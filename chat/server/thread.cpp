#include "thread.h"
static client first,last;
static int lastid;
static int res;
void threadInit(){
    first=new Client;
    last=new Client;
    first->next=last;
    last->prev=first;
}
//using state machine for client in recv/send functions would be probably a great idea
static void recvn(client c,char*buff,int len){
    int i=0;
    while(i<len){
        res=recv(c->sock,buff+i,len-i,0);
        if(res==0||res==SOCKET_ERROR) throw "recv error";
        i+=res;
    }
}
static int recvi(client c){
    int i;
    recvn(c,(char*)&i,sizeof(i));
    if(res==0||res==SOCKET_ERROR) throw "recv error";
    return i;
}
static char* recvs(client c,int min,int max){
    int i=recvi(c);
    printf("length = %i\n",i);
    if(i<min)throw "data too short";
    if(i>max)throw "data too long";
    char*s=new char[max+1];
    recvn(c,s,i);
    s[i]=0;
    return s;
}
static void sendi(client c,int i){
    send(c->sock,(const char*)&i,sizeof(i),0);
}
static void sends(client c,char*s){
    int l=strlen(s);
    sendi(c,l); send(c->sock,s,l,0);
}
static void senderr(client c,int errcode){
    sendi(c,MSGERROR); sendi(c,errcode);
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
        int msgid=recvi(c);
        printf("msgid=%i\n",msgid);
        switch(msgid){
            case MSGNAME:{
                if(c->loggedin) throw "already logged in";
                char*name=recvs(c,NAMEMIN,NAMEMAX);
                if(nameinuse(name)){
                    printf("%s: name already in use\n",name);
                    delete[]name;
                    senderr(c,NAMEINUSE); break;
                }
                strcpy(c->name,name); delete[]name;
                c->loggedin=1;
                printf("Client %i logged in with name %s\n",c->id,c->name);
                senderr(c,NAMEOK);
                break;
            }
            default:{
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

