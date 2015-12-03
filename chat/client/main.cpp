#include <winsock2.h>
#include <cstdio>
#define BUFFSIZE 1024
static int res;
static SOCKET sock;
static sockaddr_in addr;
static int port=27015;
static char out[BUFFSIZE];
static char in[BUFFSIZE];
static int buffsize=BUFFSIZE;
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
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    addr.sin_port=htons(port);
    //connect
    res=connect(sock,(const sockaddr*)&addr,sizeof(addr));
    if(res!=NO_ERROR)throw "connect error!";
}
static void uninit(){
    if(sock)closesocket(sock);
    WSACleanup();
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
        res=recv(sock,in,buffsize,0);
        if(res==0) break; //connection has been closed
        else if(res==SOCKET_ERROR) throw "recv error!";
        else printf("%s",in);
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
