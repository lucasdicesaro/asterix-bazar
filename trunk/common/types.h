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

#define DEBUG_LEVEL "DEBUG"
#define INFO_LEVEL "INFO"
#define WARN_LEVEL "WARN"
#define ERROR_LEVEL "ERROR"


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
	INIT = 1,
	DO_LOOKUP,
	DO_REPLY,	
	DO_BUY,
	DO_LOOKUP_FORWARD,
	DO_REPLY_FORWARD,
	DO_START_BUY,
	SET_CUR_PROD_COMPRA,
	SET_CUR_PROD_VENTA,
	SET_STOCK_SAL,
	SET_STOCK_PESCADO, 
	SET_STOCK_VERDURA,
	QUIT,
	RUN_TIMEOUT,
	LOOP,
	CLIENT_MSG,
	FIRST_BROADCAST,
	BROADCAST,
	SEND_TO_SOCKET,	
	LS_LISTEN,
	LS_CLOSE_CONNECTIONS,
	LS_ADD_SOCKET,
	LS_RM_SOCKET,
	RT_ADD_NODO_CLIENTE,
	RT_RM_NODO_CLIENTE,
	RT_ADD_NODO_SERVIDOR,
	RT_RM_NODO_SERVIDOR,	
	KB_SHOW_MENU
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

//typedef struct
//{
//    char nombre_nodo[NOMBRE_NODO_SIZE];
//    int id_socket;
//} RouterDS;



#endif //_TYPES_H_
