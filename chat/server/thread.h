#ifndef THREAD_H
#define THREAD_H
#include <windows.h>
#include <cstdio>
#define BUFFSIZE 1024
struct Client{
    SOCKET sock;
    int id;
    Client*prev,*next;
    char buff[BUFFSIZE];
};
typedef Client* client;
void threadInit();
DWORD WINAPI threadNew(void*);
#endif // THREAD_H
