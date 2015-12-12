#include <winsock2.h>
#include <cstdio>
#include "constants.h"
static int res;
static SOCKET sock;
static sockaddr_in addr;
static char out[MSGMAX+1];
static char in[MSGMAX+1];
static char name[NAMEMAX+1];
static char loggedin;
static void init(){
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
static void uninit(){
    if(sock)closesocket(sock);
    WSACleanup();
}
static void recvn(char*buff,int len){
    int i=0;
    while(i<len){
        res=recv(sock,buff+i,len-i,0);
        if(res==0||res==SOCKET_ERROR) throw "recv error";
        i+=res;
    }
}
static int recvi(){
    int i;
    recvn((char*)&i,sizeof(i));
    if(res==0||res==SOCKET_ERROR) throw "recv error";
    return i;
}
static char* recvs(int min,int max){
    int i=recvi();
    if(i<min)throw "data too short";
    if(i>max)throw "data too long";
    char*s=new char[max+1];
    recvn(s,i);
    s[i]=0;
    return s;
}
static void sendi(int i){
    send(sock,(const char*)&i,sizeof(i),0);
}
static void sends(char*s){
    int l=strlen(s);
    sendi(l); send(sock,s,l,0);
}
static void login(){
    loggedin=0;
    char format[100];
    sprintf(format,"%%%is",1+NAMEMAX);
    while(!loggedin){
        printf("Please select a name (%i to %i characters):\n",NAMEMIN,NAMEMAX);
        scanf(format,name);
        int l=strlen(name);
        if(l<NAMEMIN){
            printf("Name must be at least %i characters long!\n",NAMEMIN);
            continue;
        }
        //send msg
        printf("Logging in...\n");
        sendi(MSGNAME); sends(name);
        //wait for answer
        int msgid=recvi();
        if(msgid!=MSGERROR) throw "unexpected message id";
        int errid=recvi();
        if(errid==NAMEOK){
            printf("Login success!\n");
            loggedin=1;
        }
        if(errid==NAMEINUSE) printf("Name is already in use!\n");
    }
}
static DWORD WINAPI read(void*socketData){
    SOCKET sock=*(SOCKET*)socketData;
    int res;
    while(1){
        scanf("\n%[^\n]",out);
        int len=1+strlen(out);
        res=send(sock,out,len,0);
        if(res==SOCKET_ERROR)throw "send error!";
    }
}
static void write(){
    int res;
    while(1){
        int msgid=recvi();
        printf("Received msg with id %i\n",msgid);
    }
}
static void run(){
    DWORD dw;
    HANDLE h;
    h=CreateThread(0,4096,read ,&sock,0,&dw);
    write();
}
int main(){
    try{
        init();
        login();
        run();
    }
    catch(const char*msg){
        printf("%s\n",msg);
        printf("Error code: %i\n",WSAGetLastError());
        uninit();
        return 1;
    }
    uninit();
    return 0;
}
