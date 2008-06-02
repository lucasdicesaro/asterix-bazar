#ifndef _TYPES_H_
#define _TYPES_H_

#include <iostream>

#define INFINITE (-1)

#define PORT_TCP (2552)
#define PORT_UDP (2551)

#define MAX_CONNECTED (10)

#define BUFFER_SIZE 2000

#define NOMBRE_NODO_SIZE (30)

#define SOCK_ERRONEO (-1)

#define DEBUG_LEVEL "DEBUG "
#define INFO_LEVEL "INFO "
#define ERROR_LEVEL "ERROR "

#define PRODUCTO_SAL ("sal")
#define PRODUCTO_PESCADO ("pescado")
#define PRODUCTO_VERDURA ("verdura")

#define CODIGO_LOOKUP ("lookup")
#define CODIGO_REPLY ("reply")
#define CODIGO_BUY ("buy")

//typedef std::basic_string<char> bstring;

//typedef struct
//{
//	int sock;
//	char ip[32];
//}ConnectDS;

typedef enum
{
	KEY_UP = 1,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_LEFT,
	KEY_SPACE,
	KEY_ESCAPE,
	INIT,
	DO_LOOKUP,
	DO_REPLY,	
	DO_BUY,				
	QUIT,
	RUN_TIMEOUT,
	LOOP,
	CLIENT_MSG,
	NEWDATA,
	PLAYER_ID,
	PRE_QUIT,
	BROADCAST,
	BD_ADD_IP,
	BD_RM_IP,
	BD_KILL_ALL,
	LS_LISTEN,
	LS_CLOSE_CONNECTIONS,
	SND_BLOCKED
} EventID;

typedef struct
{
	EventID id;
	const void* tag;
} Event;

typedef struct
{
    int intentos_reconexion;
    int delay_reconexion;
    int hopcount;
} ReconnectParamsDS;

typedef struct
{
        char 		  nombre[NOMBRE_NODO_SIZE];
        char		  ip[30];
        int			  port;
        char		  vecino1[NOMBRE_NODO_SIZE];
        char		  vecino2[NOMBRE_NODO_SIZE];        
}ConfigDS;


#endif //_TYPES_H_
