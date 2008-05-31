#ifndef _TYPES_H_
#define _TYPES_H_

#include <iostream>

#define INFINITE (-1)

#define PORT_TCP (2552)
#define PORT_UDP (2551)

#define MAX_CONNECTED (10)

#define BUFFER_SIZE 100

#define NOMBRE_NODO_SIZE (30)

#define SOCK_ERRONEO (-1)
#define INTENTOS_RECONEXION (3)
#define DELAY_RECONEXION (20)

#define DEBUG_LEVEL "DEBUG "
#define INFO_LEVEL "INFO "
#define ERROR_LEVEL "ERROR "

#define XML_WELCOME_ROOT_ELEMENT "event"
#define XML_WELCOME_FIRST_ELEMENT "ticket"

typedef std::basic_string<char> bstring;

typedef struct
{
	int sock;
	char ip[32];
}ConnectDS;

typedef enum
{
	KEY_UP = 1,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_LEFT,
	KEY_SPACE,
	KEY_ESCAPE,
	INIT,
	QUIT,
	RUN_TIMEOUT,
	LOOP,
	BUILD_MSG,
	SEND_BROADCAST_MSG,
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
	SND_BLOCKED,
	LOOKUP,
	REPLAY,
	BUY
} EventID;

typedef struct
{
	EventID id;
	const void* tag;
} Event;

typedef struct
{
        char 		  nombre[NOMBRE_NODO_SIZE];
        char		  ip[30];
        int			  port;
        char		  vecino1[NOMBRE_NODO_SIZE];
        char		  vecino2[NOMBRE_NODO_SIZE];
        unsigned int  param_3;
        unsigned int  param_4;
}ConfigDS;


#endif //_TYPES_H_
