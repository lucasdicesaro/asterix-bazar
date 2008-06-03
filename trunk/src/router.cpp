

#include "common/tools.h"
#include "common/types.h"
#include "common/socket_util.h"
#include "router.h"
#include "logic.h"
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <signal.h>

Router* Router::single_instance = NULL;

extern char *nombre_nodo;

/**
* Constructors
*/
Router::Router()
{
	ReconnectParamsDS *reconnectParams = Tools::instance()->get_reconnect_params();
	intentos_reconexion = reconnectParams->intentos_reconexion;
	delay_reconexion = reconnectParams->delay_reconexion;
}

Router::~Router()
{
}

Router* Router::instance()
{
	if (!single_instance)
		single_instance = new Router;
	return single_instance;
}

void Router::on_event(const Event& ev)
{
	switch (ev.id)
	{
		case INIT:
			Tools::debug("Router: on_event: INIT:");			
			start_connections();
			break;			
			
		case BROADCAST:
			Tools::debug("Router: on_event: BROADCAST:");
			send_broadcast_message(ev.tag);
			break;	
			
		case SEND_TO_SOCKET:
			Tools::debug("Router: on_event: SEND_TO_SOCKET:");
			send_to_socket_message(ev.tag);
			break;
			
		default:
			Tools::debug("Router: on event: *UNKNOWN*.");
			break;
	}
}


void Router::start_connections() 
{	
	char logBuffer[BUFFER_SIZE];
	char *echoBuffer = new char[BUFFER_SIZE];
	
	Tools* tools = Tools::instance();
	ConfigDS* my_config = tools->get_info_nodo(nombre_nodo);
	
	ConfigDS* vecino1_config = tools->get_info_nodo(my_config->vecino1);		
	if (vecino1_config == NULL) 
	{
		sprintf(logBuffer, "Router: El nombre de nodo %s, no existe en el archivo de configuracion", my_config->vecino1);
		Tools::info(logBuffer);
		exit(1);
	}
	
	ConfigDS* vecino2_config = tools->get_info_nodo(my_config->vecino2);
	if (vecino2_config == NULL) 
	{
		sprintf(logBuffer, "Router: El nombre de nodo %s, no existe en el archivo de configuracion", my_config->vecino2);
		Tools::info(logBuffer);
		exit(1);
	}	
	
	sock_vecino1 = SocketUtil::cliente_abrir_conexion_tcp (vecino1_config->ip, vecino1_config->port);
	if (sock_vecino1 != SOCK_ERRONEO)
	{			
		std::string mensaje = get_connect_msg();
		SocketUtil::enviar_mensaje(sock_vecino1, mensaje);		
	}
	
	sock_vecino2 = SocketUtil::cliente_abrir_conexion_tcp (vecino2_config->ip, vecino2_config->port);	
	if (sock_vecino2 != SOCK_ERRONEO)
	{		
		std::string mensaje = get_connect_msg();
		SocketUtil::enviar_mensaje(sock_vecino2, mensaje);			
	}
	
	bool conexiones_incompletas = sock_vecino1 == SOCK_ERRONEO || sock_vecino2 == SOCK_ERRONEO;
	if (intentos_reconexion > 0 && conexiones_incompletas)
	{
		//Tools::info("Router: No se pudo lograr la comunicacion con al menos uno de los dos vecinos");
		sprintf(logBuffer, "Router: Intento de reconexion nro %d. Se espera un delay de %d", intentos_reconexion, delay_reconexion);
		Tools::info(logBuffer);
		
		intentos_reconexion--;
		sleep(delay_reconexion);
		
		Event ev;
		ev.id = INIT;		
		this->post_event (ev, true);
	}
	else 	// Cuando se agotan los intentos de reconexion o ambos sockets hayan logrado conectarse	
	{
		if (intentos_reconexion == 0)
		{
			if (sock_vecino1 == SOCK_ERRONEO && sock_vecino2 == SOCK_ERRONEO)				
			{
				// No se establecio conexion con ninguno de los dos vecinos
				Tools::info("Router: Se agotaron los intentos de reconexion. Presiones Ctrl+C e intente mas tarde.");
			}
		}
		if (!conexiones_incompletas) 
		{
			Tools::info("Router: Se logro la conexion con ambos vecinos");
		}
				
		if (sock_vecino1 != SOCK_ERRONEO || sock_vecino2 != SOCK_ERRONEO) 
		{
			// Al menos hay una conexion con alguno de los dos vecinos: Comenzar a Comprar / Vender
			Event ev;
			ev.id = DO_LOOKUP;		
			Logic::instance()->post_event(ev, true);
		}
	}
}


void Router::send_broadcast_message(const void* msg)
{
	char logBuffer[BUFFER_SIZE];		// Buffer for log
	std::string mensaje = (const char*)msg;
		
	if (sock_vecino1 != SOCK_ERRONEO)
	{				
		SocketUtil::enviar_mensaje(sock_vecino1, mensaje);
	}
	if (sock_vecino2 != SOCK_ERRONEO)
	{
		SocketUtil::enviar_mensaje(sock_vecino2, mensaje);
	}
	sprintf(logBuffer, "Router: Se envio el mensaje [%s] a los vecinos", mensaje.c_str());
	Tools::info(logBuffer);		
}

void Router::send_to_socket_message(const void* msg)
{
	char logBuffer[BUFFER_SIZE];
	std::string mensaje = (const char*)msg;
	int socket = 3; // TODO determinar como obtener el socket del ultimo nodo
	
	
	//SocketUtil::enviar_mensaje(socket, mensaje);	
	sprintf(logBuffer, "Router: Se envio el mensaje [%s] solo al socket [%d]", mensaje.c_str(), socket);
	Tools::info(logBuffer);		
}



std::string Router::get_connect_msg()
{
	char logBuffer[BUFFER_SIZE];
	//memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Router: Enviando Handshake [%s]", nombre_nodo);
	Tools::info(logBuffer);			
	return Tools::duplicate(nombre_nodo);
}



void Router::close_TCP_connections()
{
	// Inicia la rutina de salida
	close(sock_vecino1);
	close(sock_vecino2);
	
	//Event ev;
	//ev.id  = QUIT;
	//this->post_event(ev, true);
}



