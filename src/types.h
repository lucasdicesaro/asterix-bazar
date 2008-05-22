#ifndef _TYPES_H_
#define _TYPES_H_

#include <iostream>

#define INFINITE (-1)

#define PORT_TCP (2552)
#define PORT_UDP (2551)

#define MAX_CONNECTED (4)

#define SERVER_DESERTION ("blood_bath")
#define CLIENT_DESERTION ("death_chain")


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
	ENVIAR,
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
        char 		  nombre[20];
        char		  ip[30];
        unsigned int  param_3;
        unsigned int  param_4;
}ConfigDS;


#endif //_TYPES_H_
