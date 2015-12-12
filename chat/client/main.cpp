#include "network.h"
static char out[MSGMAX+1];
static char in[MSGMAX+1];
static char name[NAMEMAX+1];
static char loggedin;
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
static DWORD WINAPI read(void*param){
    while(1){
        scanf("\n%[^\n]",out);
        if(out[0]=='/'){
            //command
            char cmd[MSGMAX+1]; sscanf(out,"/%s",cmd);
            if(!strcmp(cmd,"join")){
                char room[ROOMMAX+1]; sscanf(out,"/%s %s",cmd,room);
                printf("Trying to join room %s...\n",room);
                sendi(MSGJOIN); sends(room);
            }
        }
        else{
            sendi(MSGALL); sends(out);
        }
    }
    return 0;
}
static void error(int errid){
    switch(errid){
        case JOINOK:{
            printf("Joined room\n");
            break;
        }
        case ROOMFULL:{
            printf("Room is full.\n");
            break;
        }
        case INROOM:{
            printf("You are already in that room!\n");
            break;
        }
    }
}
static void write(){
    while(1){
        int msgid=recvi();
        switch(msgid){
            case MSGFROM:{
                char*from=recvs(NAMEMIN,NAMEMAX);
                char* msg=recvs(MSGMIN, MSGMAX);
                printf("%s: %s\n",from,msg);
                delete[]from; delete[]msg;
                break;
            }
            case MSGERROR:{
                error(recvi());
                break;
            }
            default: throw "unexpected message id";
        }
    }
}
static void run(){
    DWORD dw;
    CreateThread(0,4096,read ,0 ,0,&dw);
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
