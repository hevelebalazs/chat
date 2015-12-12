#include "network.h"
static int res;
client __thread selc;
void recvn(char*buff,int len){
    int i=0;
    while(i<len){
        res=recv(selc->sock,buff+i,len-i,0);
        if(res==0||res==SOCKET_ERROR) throw "recv error";
        i+=res;
    }
}
int recvi(){
    int i;
    recvn((char*)&i,sizeof(i));
    if(res==0||res==SOCKET_ERROR) throw "recv error";
    return i;
}
char* recvs(int min,int max){
    int i=recvi();
    if(i<min)throw "data too short";
    if(i>max)throw "data too long";
    char*s=new char[max+1];
    recvn(s,i);
    s[i]=0;
    return s;
}

void sendn(char*buff,int len){
    int i=0;
    while(i<len){
        res=send(selc->sock,buff+i,len-i,0);
        if(res==0||res==SOCKET_ERROR) throw "send error";
        i+=res;
    }
}
void sendi(int i){
    sendn((char*)&i,sizeof(i));
}
void sends(char*s){
    int l=strlen(s);
    sendi(l); sendn(s,l);
}
void senderr(int errcode){
    sendi(MSGERROR); sendi(errcode);
}
