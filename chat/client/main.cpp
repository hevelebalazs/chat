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
                sendi(MSGJOIN); sends(room);
            }
            else if(!strcmp(cmd,"leave")){
                char room[ROOMMAX+1]; sscanf(out,"/%s %s",cmd,room);
                sendi(MSGLEAVE); sends(room);
            }
            else if(!strcmp(cmd,"pm")){
                char name[NAMEMAX+1],msg[MSGMAX+1]; sscanf(out,"/%s %s %[^\0]",cmd,name,msg);
                sendi(MSGTO); sends(name); sends(msg);
            }
            else if(!strcmp(cmd,"room")){
                char room[ROOMMAX+1],msg[MSGMAX+1]; sscanf(out,"/%s %s %[^\0]",cmd,room,msg);
                sendi(MSGROOM); sends(room); sends(msg);
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
        case NOSUCHUSER:{
            printf("No such user found!\n");
            break;
        }
        case NOTINROOM:{
            printf("You are not in that room! Use /join [roomname] to join.\n");
            break;
        }
        default:{
            printf("Error message: %i\n",errid);
            throw "invalid error message";
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
            case MSGROOM:{
                char*room=recvs(ROOMMIN,ROOMMAX);
                char*from=recvs(NAMEMIN,NAMEMAX);
                char* msg=recvs(MSGMIN,MSGMAX);
                printf("[%s] %s: %s\n",room,from,msg);
                break;
            }
            case MSGJOIN:{
                char*room=recvs(ROOMMIN,ROOMMAX);
                char*user=recvs(NAMEMIN,NAMEMAX);
                printf("[%s] %s has joined.\n",room,user);
                break;
            }
            case MSGLEAVE:{
                char*room=recvs(ROOMMIN,ROOMMAX);
                char*user=recvs(NAMEMIN,NAMEMAX);
                printf("[%s] %s has left.\n",room,user);
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
