#ifndef CONSTANTSH
#define CONSTANTSH

#define NTWPORT 27015
#define NTWADDR "127.0.0.1"

#define NAMEMIN 2
#define NAMEMAX 10
#define MSGMIN 1
#define MSGMAX 1024

//message types
enum{MSGNAME,MSGERROR};
//error types
enum{NAMEOK,NAMEINUSE};

#endif
