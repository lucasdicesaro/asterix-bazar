#ifndef _TYPES_H_
#define _TYPES_H_

#include <iostream>

#define ABSTRACT 0

#define INFINITE (-1)

#define PORT_TCP (2552)
#define PORT_UDP (2551)

#define MAX_CONNECTED (10)

#define BUFFER_SIZE 2000

#define NOMBRE_NODO_SIZE (20)

#define SOCK_ERRONEO (-1)

#define DEFAULT_WAIT_SECONDS (5)
#define DEFAULT_WAIT_MILISECONDS (0)

#define DEBUG_LEVEL "DEBUG"
#define INFO_LEVEL "INFO"
#define WARN_LEVEL "WARN"
#define ERROR_LEVEL "ERROR"

#define P2P_CONNECT "p2p_connect"

#define SEPARADOR ("|")

#define PRODUCTO_SAL ("sal")
#define PRODUCTO_PESCADO ("pescado")
#define PRODUCTO_VERDURA ("verdura")

#define CODIGO_LOOKUP ("lookup")
#define CODIGO_REPLY ("reply")
#define CODIGO_BUY ("buy")
#define CODIGO_BUY_OK ("buy_ok")
#define CODIGO_BUY_ERR ("buy_err")

typedef enum
{
	INIT = 1,
	DO_LOOKUP,
	DO_REPLY,	
	DO_BUY,
	DO_BUY_REPLY_OK,
	DO_BUY_REPLY_ERR,			
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
	LS_LISTEN,
	LS_CLOSE_CONNECTIONS,
	LS_ADD_SOCKET,
	LS_ADD_SOCKET_P2P,
	LS_RM_SOCKET,
	LS_RM_SOCKET_P2P,
	RT_FIRST_BROADCAST,
	RT_BROADCAST,
	RT_SEND_TO_SOCKET,
	RT_START_P2P_CONNECT,
	RT_CLOSE_P2P_CONNECT,
	RT_SEND_TO_P2P_SOCKET,									
	RT_ADD_NODO_CLIENTE,
	RT_RM_NODO_CLIENTE,
	RT_ADD_NODO_SERVIDOR,
	RT_RM_NODO_SERVIDOR,	
	RT_SET_SOCKET_P2P,
	RT_UNSET_SOCKET_P2P,
	KB_SHOW_MENU, 
   	KB_SHOW_STOCK,
   	OUTPUT_STATUS_DEBUG,
   	OUTPUT_STATUS_INFO,
   	OUTPUT_STATUS_WARN,
   	OUTPUT_STATUS_ERROR,
   	OUTPUT_STOCK,
   	OUTPUT_MENU
} EventID;


typedef enum
{
	LOOKUP_ENVIADO=0,
	REPLY_RECIBIDO
} EstadoPedidoID;

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
    std::string producto_compra;
    std::string producto_venta;
} CurrentStockDS;

typedef struct
{
        char 		  nombre[NOMBRE_NODO_SIZE];
        char		  ip[30];
        int			  port;
        char		  vecino1[NOMBRE_NODO_SIZE];
        char		  vecino2[NOMBRE_NODO_SIZE];        
} ConfigDS;


#endif //_TYPES_H_
