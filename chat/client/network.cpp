#include <winsock2.h>
#include <stdio.h>
#include "network.h"
static SOCKET sock;
static sockaddr_in addr;
static int res;
void init(){
    //init network
    WSAData wsaData;
    res=WSAStartup(MAKEWORD(2,2),&wsaData);
    if(res!=NO_ERROR)throw "WSAStartup error!";
    //init socket
    sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sock==INVALID_SOCKET)throw "socket error!";
    //init address
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(NTWADDR);
    addr.sin_port=htons(NTWPORT);
    //connect
    res=connect(sock,(const sockaddr*)&addr,sizeof(addr));
    if(res!=NO_ERROR)throw "connect error!";
}
void uninit(){
    if(sock)closesocket(sock);
    WSACleanup();
}
//receive functions
void recvn(char*buff,int len){
    int i=0;
    while(i<len){
        res=recv(sock,buff+i,len-i,0);
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
//send functions
void sendn(char*buff,int len){
    int i=0;
    while(i<len){
        res=send(sock,buff+i,len-i,0);
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
