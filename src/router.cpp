

#include "common/tools.h"
#include "common/types.h"
#include "common/socket_util.h"
#include "router.h"
#include "logic.h"
#include "listener.h"
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
	sock_vecino1 = SOCK_ERRONEO;
	sock_vecino2 = SOCK_ERRONEO;	
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
	
	ConfigDS* my_config = Tools::instance()->get_info_nodo(nombre_nodo);
	
	if (sock_vecino1 == SOCK_ERRONEO)
	{
		sock_vecino1 = try_connection(my_config->vecino1);
		/*ConfigDS* vecino1_config = tools->get_info_nodo(my_config->vecino1);		
		if (vecino1_config == NULL) 
		{
			sprintf(logBuffer, "Router: El nombre de nodo %s, no existe en el archivo de configuracion", my_config->vecino1);
			Tools::info(logBuffer);
			exit(1);
		}
		
		sock_vecino1 = SocketUtil::cliente_abrir_conexion_tcp (vecino1_config->ip, vecino1_config->port);
		if (sock_vecino1 != SOCK_ERRONEO)
		{			
			std::string mensaje = get_connect_msg();
			SocketUtil::enviar_mensaje(sock_vecino1, mensaje);		
		}*/
	}
	else
	{
		sprintf(logBuffer, "Router: start_connections: El vecino [%s], ya tiene socket", my_config->vecino1);
		Tools::debug(logBuffer);
	}
	
	if (sock_vecino2 == SOCK_ERRONEO)
	{
		sock_vecino2 = try_connection(my_config->vecino2);
		/*ConfigDS* vecino2_config = tools->get_info_nodo(my_config->vecino2);
		if (vecino2_config == NULL) 
		{
			sprintf(logBuffer, "Router: El nombre de nodo %s, no existe en el archivo de configuracion", my_config->vecino2);
			Tools::info(logBuffer);
			exit(1);
		}	
		
		sock_vecino2 = SocketUtil::cliente_abrir_conexion_tcp (vecino2_config->ip, vecino2_config->port);	
		if (sock_vecino2 != SOCK_ERRONEO)
		{		
			std::string mensaje = get_connect_msg();
			SocketUtil::enviar_mensaje(sock_vecino2, mensaje);			
		}*/
	}
	else
	{
		sprintf(logBuffer, "Router: start_connections: El vecino [%s], ya tiene socket", my_config->vecino2);
		Tools::debug(logBuffer);
	}
	
	bool conexiones_incompletas = sock_vecino1 == SOCK_ERRONEO || sock_vecino2 == SOCK_ERRONEO;
	//Tools::debug_label_value ("Router: start_connections: conexiones_incompletas: ", conexiones_incompletas);
	if (intentos_reconexion > 0 && conexiones_incompletas)
	{
		// No se pudo lograr la comunicacion con al menos uno de los dos vecinos
		sprintf(logBuffer, "Router: start_connections: Intento de reconexion nro %d. Se espera un delay de %d", intentos_reconexion, delay_reconexion);
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
				Tools::info("Router: start_connections: Se agotaron los intentos de reconexion. Presiones Ctrl+C e intente mas tarde.");
			}
		}
		if (!conexiones_incompletas) 
		{
			Tools::info("Router: start_connections: Se logro la conexion con ambos vecinos");
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


int Router::try_connection(const char *nombre_nodo) 
{
	int socket;
	char logBuffer[BUFFER_SIZE];
	
	ConfigDS* vecino_config = Tools::instance()->get_info_nodo(nombre_nodo);		
	if (vecino_config == NULL) 
	{
		sprintf(logBuffer, "Router: try_connection: El nombre de nodo %s, no existe en el archivo de configuracion", nombre_nodo);
		Tools::debug(logBuffer);
		exit(1);
	}
	
	socket = SocketUtil::cliente_abrir_conexion_tcp (vecino_config->ip, vecino_config->port);
	if (socket != SOCK_ERRONEO)
	{			
		std::string mensaje = get_connect_msg();
		SocketUtil::enviar_mensaje(socket, mensaje);		
	}
	return socket;
}


void Router::send_broadcast_message(const void* msg)
{
	char logBuffer[BUFFER_SIZE];		// Buffer for log
	std::string mensaje = (const char*)msg;
	Tools::debug("Router::send_broadcast_message: Se va a enviar el mensaje a los vecinos");
	if (sock_vecino1 != SOCK_ERRONEO)
	{				
		Tools::debug("Router::send_broadcast_message: se enviara por socket1");
		SocketUtil::enviar_mensaje(sock_vecino1, mensaje);
		//Tools::debug("Router::send_broadcast_message: enviado por socket1");		
	}
	if (sock_vecino2 != SOCK_ERRONEO)
	{
		Tools::debug("Router::send_broadcast_message: se enviara por socket2");
		SocketUtil::enviar_mensaje(sock_vecino2, mensaje);
		//Tools::debug("Router::send_broadcast_message: enviado por socket2");				
	}
	if (sock_vecino1 != SOCK_ERRONEO || sock_vecino2 != SOCK_ERRONEO)
	{
		sprintf(logBuffer, "Router: send_broadcast_message: Se envio el mensaje [%s] a los vecinos", mensaje.c_str());		
		Tools::debug(logBuffer);		
	}
	else
	{
		sprintf(logBuffer, "Router: send_broadcast_message: No se envio el mensaje [%s] porque no hay sockets_vecinos", mensaje.c_str());		
		Tools::debug(logBuffer);		
	}
}

void Router::send_to_socket_message(const void* msg)
{
	char logBuffer[BUFFER_SIZE];
	std::string nombre_nodo = Logic::instance()->get_next_node_name(msg);
	sprintf(logBuffer, "Router: send_to_socket_message: Se obtuvo de get_next_node_name el nombre [%s]", nombre_nodo.c_str());		
	Tools::debug(logBuffer);
	
	int socket = Listener::instance()->get_socket_from_client(nombre_nodo);
	//int socket = *sock;
	sprintf(logBuffer, "Router: send_to_socket_message: Se obtuvo de get_socket_from_client el socket [%d]", socket);		
	Tools::debug(logBuffer);
	
	std::string mensaje = Logic::instance()->get_mensaje_as_string(msg);
	sprintf(logBuffer, "Router: send_to_socket_message: Se obtuvo de get_mensaje_as_string el mensaje [%s]", mensaje.c_str());		
	Tools::debug(logBuffer);
	
	//std::string mensaje("PUTTOOOOOOOOOO");
	//Tools::debug("Router: send_to_socket_message: LLego a puto");
	if (socket != SOCK_ERRONEO)
	{		
		SocketUtil::enviar_mensaje(socket, mensaje);	
		sprintf(logBuffer, "Router: send_to_socket_message: Se envio el mensaje [%s] solo al socket [%d]", mensaje.c_str(), socket);		
		Tools::debug(logBuffer);
	}
	else
	{
		if (!mensaje.empty())
		{
			sprintf(logBuffer, "Router: send_to_socket_message: No se pudo mandar el mensaje [%s] individual. Socket erroneo", mensaje.c_str());		
			Tools::error(logBuffer);			
		}
		else 
		{
			Tools::error("Router: send_to_socket_message: el socket es erroneo y el mensaje esta vacio");
		}
	}
}



std::string Router::get_connect_msg()
{
	char logBuffer[BUFFER_SIZE];
	//memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Router: get_connect_msg: Enviando Handshake [%s]", nombre_nodo);
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



