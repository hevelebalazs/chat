#ifndef THREAD_H
#define THREAD_H
#include <windows.h>
#include <cstdio>
#include "../client/constants.h"
struct Client{
    SOCKET sock;
    int id;
    Client*prev,*next;
    char name[NAMEMAX+1];
    char msg[MSGMAX+1];
    char loggedin;
};
typedef Client* client;
void threadInit();
DWORD WINAPI threadNew(void*);
#endif // THREAD_H
