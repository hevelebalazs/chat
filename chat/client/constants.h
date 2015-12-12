#ifndef CONSTANTSH
#define CONSTANTSH

#define NTWPORT 27015
#define NTWADDR "127.0.0.1"

#define ROOMSIZE 100

#define NAMEMIN 2
#define NAMEMAX 10
#define ROOMMIN 2
#define ROOMMAX 10
#define MSGMIN 1
#define MSGMAX 1024

//message types
enum{MSGNAME,MSGERROR,MSGALL,MSGFROM,MSGJOIN};
//error types
enum{NAMEOK,NAMEINUSE,INROOM,JOINOK,ROOMFULL};

#endif
