#include <winsock2.h>
#include <cstdio>
#include "thread.h"
static int res;
static SOCKET listenSocket;
static sockaddr_in address;
static int port=27015;
void init(){
    //init threading
    threadInit();
    //init network
    WSADATA wsaData;
    res=WSAStartup(MAKEWORD(2,2),&wsaData);
    if(res!=NO_ERROR)throw "WSAStartup error!";
    //init socket
    listenSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(listenSocket==INVALID_SOCKET)throw "socket error!";
    //init address
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=inet_addr("127.0.0.1");
    address.sin_port=htons(port);
    res=bind(listenSocket,(SOCKADDR*)&address,sizeof(address));
    if(res==SOCKET_ERROR)throw "bind error!";
    //listen
    res=listen(listenSocket,2);
    if(res==SOCKET_ERROR)throw "Listen error!";
}
void uninit(){
    if(listenSocket)closesocket(listenSocket);
    WSACleanup();
}
void run(){
    SOCKET* clientSocket=new SOCKET;
    *clientSocket=accept(listenSocket,0,0);
    if(*clientSocket==INVALID_SOCKET)throw "accept error!";
    //client connected
    DWORD dw;
    HANDLE handle=CreateThread(0,4096,threadNew,clientSocket,0,&dw);
    if(handle==INVALID_HANDLE_VALUE)throw "handle error!";
}
int main(){
    try{
        init();
        while(1)run();
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
