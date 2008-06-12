

#include "common/tools.h"
#include "common/types.h"
#include "common/socket_util.h"
#include "router.h"
#include "logic.h"
#include "listener.h"
#include "keyboard.h"
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <signal.h>

Router* Router::single_instance = NULL;

extern char *nombre_nodo;
extern bool en_operacion;

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
	sock_p2p = SOCK_ERRONEO;	
	show_menu = false;
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
	char logBuffer[BUFFER_SIZE];
	std::string nombre_nodo;
	int id_socket = SOCK_ERRONEO;
	char *nodo_socket = NULL;
	switch (ev.id)
	{
		case INIT:
			Tools::debug("Router: on_event: INIT:");			
			start_connections();
			break;			
			
		case RT_FIRST_BROADCAST:
			Tools::debug("Router: on_event: FIRST_BROADCAST:");
			send_first_broadcast_message(ev.tag);
			break;	
			
		case RT_BROADCAST:
			Tools::debug("Router: on_event: BROADCAST:");
			send_broadcast_message(ev.tag);
			break;	
			
		case RT_SEND_TO_SOCKET:
			Tools::debug("Router: on_event: SEND_TO_SOCKET:");
			send_to_next_node_message(ev.tag);
			break;
			
		case RT_SEND_TO_P2P_SOCKET:
			Tools::debug("Router: on_event: RT_SEND_TO_P2P_SOCKET:");
			send_to_p2p_socket_message(ev.tag);
			break;			
			
		case RT_START_P2P_CONNECT:
			Tools::debug("Router: on_event: RT_START_P2P_CONNECT:");
			start_p2p_connect(ev.tag);
			break;	
			
		case RT_CLOSE_P2P_CONNECT:
			Tools::debug("Router: on_event: RT_CLOSE_P2P_CONNECT:");
			close_p2p_connect(ev.tag);
			break;			

		case RT_ADD_NODO_SERVIDOR:		
			Tools::debug("Router: on_event RT_ADD_NODO_SERVIDOR");
			
			nodo_socket = new char[BUFFER_SIZE];
			memset(nodo_socket, 0, BUFFER_SIZE);
			nodo_socket = (char*)ev.tag;
			
			nombre_nodo = strtok(nodo_socket, ",");
			id_socket = atoi(strtok(NULL , ","));
			
			add_nodo_socket_into_server_map(nombre_nodo, id_socket);
			break;
		
		case RT_RM_NODO_SERVIDOR:		
			Tools::debug("Router: on_event RT_RM_NODO_SERVIDOR");
			nodo_socket = (char*)ev.tag;			
			id_socket = atoi(nodo_socket);
			
			nombre_nodo = find_in_server_sockets_nodo_nombre(id_socket);
			
			sprintf(logBuffer, "Router: start_connections: nombre_nodo [%s], retornado por find_nodo_nombre_from_id_socket", nombre_nodo.c_str());
			Tools::debug(logBuffer);			
			
			rm_nodo_socket_from_server_map(nombre_nodo);
			break;					

		case RT_ADD_NODO_CLIENTE:		
			Tools::debug("Router: on_event RT_ADD_NODO_CLIENTE");
			
			nodo_socket = new char[BUFFER_SIZE];
			memset(nodo_socket, 0, BUFFER_SIZE);
			nodo_socket = (char*)ev.tag;
			
			nombre_nodo = strtok(nodo_socket, ",");
			id_socket = atoi(strtok(NULL , ","));
			
			add_nodo_socket_into_client_map(nombre_nodo, id_socket);
			break;
		
		case RT_RM_NODO_CLIENTE:		
			Tools::debug("Router: on_event RT_RM_NODO_CLIENTE");
			nodo_socket = (char*)ev.tag;			
			id_socket = atoi(nodo_socket);
			
			nombre_nodo = find_in_client_sockets_nodo_nombre(id_socket);
			sprintf(logBuffer, "Router: start_connections: nombre_nodo [%s], retornado por find_nodo_nombre_from_id_socket", nombre_nodo.c_str());
			Tools::debug(logBuffer);			

			rm_nodo_socket_from_client_map(nombre_nodo);
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
	}
	else
	{
		sprintf(logBuffer, "Router: start_connections: El vecino [%s], ya tiene socket", my_config->vecino1);
		Tools::debug(logBuffer);
	}
	
	if (sock_vecino2 == SOCK_ERRONEO)
	{
		sock_vecino2 = try_connection(my_config->vecino2);
	}
	else
	{
		sprintf(logBuffer, "Router: start_connections: El vecino [%s], ya tiene socket", my_config->vecino2);
		Tools::debug(logBuffer);
	}
	
	bool conexiones_incompletas = sock_vecino1 == SOCK_ERRONEO || sock_vecino2 == SOCK_ERRONEO;
	if (intentos_reconexion > 0 && conexiones_incompletas)
	{
		// No se pudo lograr la comunicacion con al menos uno de los dos vecinos
		sprintf(logBuffer, "Router: start_connections: Intento de reconexion nro %d. Se espera un delay de %d", intentos_reconexion, delay_reconexion);
		Tools::debug(logBuffer);
		sprintf(logBuffer, "Intento de reconexion nro %d. Se espera un delay de %d", intentos_reconexion, delay_reconexion);
		Tools::debug(logBuffer);
		
		
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
				Tools::debug("Router: start_connections: Se agotaron los intentos de reconexion");				
				Tools::info("Se agotaron los intentos de reconexion. Presiones Ctrl+C e intente mas tarde.");
			}
		}
		if (!conexiones_incompletas) 
		{
			Tools::debug("Router: start_connections: Se logro la conexion con ambos vecinos");							
			Tools::info("Se logro la conexion con ambos vecinos");
		}
		
		// Solo si no se mostro aun el menu
		if (!show_menu)
		{
			show_menu = true;
			Event ev;
			ev.id = KB_SHOW_MENU;		
			Keyboard::instance()->post_event(ev, true);
		}
	}
}


int Router::try_connection(const char *nombre_nodo) 
{
	int socket;
	char logBuffer[BUFFER_SIZE];
	char socketBuffer[BUFFER_SIZE];
	char *nombre_nodo_vecino = NULL;

	ConfigDS* vecino_config = Tools::instance()->get_info_nodo(nombre_nodo);		
	if (vecino_config == NULL) 
	{
		sprintf(logBuffer, "Router: try_connection: El nombre de nodo %s, no existe en el archivo de configuracion", nombre_nodo);
		Tools::error(logBuffer);
		exit(1);
	}
	
	socket = SocketUtil::cliente_abrir_conexion_tcp (vecino_config->ip, vecino_config->port);
	if (socket != SOCK_ERRONEO)
	{			
		// Armo el mensaje con mi nombre de nodo y lo mando a mi vecino
		std::string mensaje = get_handshake_msg();
		SocketUtil::enviar_mensaje(socket, mensaje);		
				
		// Recibo el nombre de nodo de mi vecino
		nombre_nodo_vecino = new char[BUFFER_SIZE];
		memset(nombre_nodo_vecino, 0, BUFFER_SIZE);
		nombre_nodo_vecino = SocketUtil::recibir_mensaje(socket);						
		decode_rta_handshake_msg(nombre_nodo_vecino);
		
		
		memset(socketBuffer, 0, BUFFER_SIZE);
		sprintf(socketBuffer, "%d", socket);				
		// Le envio al Listener el socket que tengo con el vecino (nueva conexion)
		Event evListener;
		evListener.id = LS_ADD_SOCKET;
		evListener.tag = Tools::duplicate (socketBuffer);
		Listener::instance()->post_event(evListener, true);
				
		
		char *buffer_nodo_socket_id = new char[BUFFER_SIZE];
		memset(buffer_nodo_socket_id, 0, BUFFER_SIZE);
		sprintf(buffer_nodo_socket_id, "%s,%d", nombre_nodo_vecino, socket);
		// Le envio al Router (yo) el par "nombre_nodo_vecino,socket" para que lo agregue a su mapa
		Event evRouter;
		evRouter.id = RT_ADD_NODO_CLIENTE;
		evRouter.tag = Tools::duplicate (buffer_nodo_socket_id);
		this->post_event(evRouter, true);		
	}
	return socket;
}


void Router::send_first_broadcast_message(const void* msg)
{
	char logBuffer[BUFFER_SIZE]; // Buffer for log
	std::string mensaje_xml = (char *)msg;
	sprintf(logBuffer, "Router: send_first_broadcast_message: Se va a enviar el xml [%s]", mensaje_xml.c_str());		
	Tools::debug(logBuffer);
	
	if (sock_vecino1 != SOCK_ERRONEO)
	{				
		sock_vecino1 = SocketUtil::enviar_mensaje(sock_vecino1, mensaje_xml);
		sprintf(logBuffer, "Router: send_first_broadcast_message: Se envio el mensaje al vecino1");		
		Tools::debug(logBuffer);		
	}
	if (sock_vecino2 != SOCK_ERRONEO)
	{
		sock_vecino2 = SocketUtil::enviar_mensaje(sock_vecino2, mensaje_xml);
		sprintf(logBuffer, "Router: send_first_broadcast_message: Se envio el mensaje al vecino2");		
		Tools::debug(logBuffer);		
	}
	
	if (sock_vecino1 == SOCK_ERRONEO && sock_vecino2 == SOCK_ERRONEO)
	{
		sprintf(logBuffer, "Router: send_first_broadcast_message: No se envio el mensaje [%s] porque no hay sockets_vecinos. Si desea retomar posibles conexiones, oprima Ctrl+C", mensaje_xml.c_str());		
		Tools::warn(logBuffer);		
	}
}




void Router::send_broadcast_message(const void* msg)
{
	char logBuffer[BUFFER_SIZE];		// Buffer for log
	std::string nombre_nodo_vecino;
	
	Mensaje *mensaje = (Mensaje *)msg;	
	std::string creator_node_name = mensaje->get_creator_node_name();	
	sprintf(logBuffer, "Router: send_broadcast_message: Se obtuvo de mensaje->get_creator_node_name el nombre [%s]", creator_node_name.c_str());		
	Tools::debug(logBuffer);	
	std::string mensaje_xml = mensaje->to_string();
	sprintf(logBuffer, "Router: send_broadcast_message: Se enviara el xml [%s]", mensaje_xml.c_str());		
	Tools::debug(logBuffer);
	
	if (sock_vecino1 != SOCK_ERRONEO)
	{				
		nombre_nodo_vecino = find_in_client_sockets_nodo_nombre(sock_vecino1);

		if (nombre_nodo_vecino.compare(creator_node_name) != 0)
		{
			sock_vecino1 = SocketUtil::enviar_mensaje(sock_vecino1, mensaje_xml);
			sprintf(logBuffer, "Router: send_broadcast_message: Se envio el mensaje al vecino 1");		
			Tools::debug(logBuffer);		
		}
		else
		{
			memset(logBuffer, 0 , sizeof(logBuffer));
			sprintf(logBuffer, "Router: send_broadcast_message: No se envia el mensaje por socket_1, ya que mi vecino [%s] es quien inicio la compra", nombre_nodo_vecino.c_str());
			Tools::debug(logBuffer);
		}
	}
	if (sock_vecino2 != SOCK_ERRONEO)
	{
		nombre_nodo_vecino = find_in_client_sockets_nodo_nombre(sock_vecino2);
		
		if (nombre_nodo_vecino.compare(creator_node_name) != 0)
		{
			sock_vecino2 = SocketUtil::enviar_mensaje(sock_vecino2, mensaje_xml);
			sprintf(logBuffer, "Router: send_broadcast_message: Se envio el mensaje al vecino 2");		
			Tools::debug(logBuffer);			
		}
		else
		{
			memset(logBuffer, 0 , sizeof(logBuffer));
			sprintf(logBuffer, "Router: send_broadcast_message: No se envia el mensaje por socket_2, ya que mi vecino [%s] es quien inicio la compra", nombre_nodo_vecino.c_str());
			Tools::debug(logBuffer);
		}		
	}
	if (sock_vecino1 == SOCK_ERRONEO && sock_vecino2 == SOCK_ERRONEO)
	{
		sprintf(logBuffer, "Router: send_broadcast_message: No se envio el mensaje porque no hay sockets_vecinos. Si desea retomar posibles conexiones, oprima Ctrl+C");		
		Tools::warn(logBuffer);		
	}
}

void Router::start_p2p_connect(const void* msg)
{
	char logBuffer[BUFFER_SIZE];
	char socketBuffer[BUFFER_SIZE];	
	Mensaje *mensaje = (Mensaje *)msg;
	std::string vendedor = mensaje->get_vendedor();
	//sprintf(logBuffer, "Router: start_p2p_connect:  [%s]", mensaje_xml.c_str());		
	//Tools::debug(logBuffer);
	std::string mensaje_xml = mensaje->to_string();
	sprintf(logBuffer, "Router: start_p2p_connect: Se va a enviar el xml [%s]", mensaje_xml.c_str());		
	Tools::debug(logBuffer);
	
	sock_p2p = try_connection(vendedor.c_str());
	if (sock_p2p != SOCK_ERRONEO)
	{
		sprintf(logBuffer, "Router: start_p2p_connect: Se obtuvo de try_connection el socket [%d]", sock_p2p);		
		Tools::debug(logBuffer);
			
		memset(socketBuffer, 0, BUFFER_SIZE);
		sprintf(socketBuffer, "%d", sock_p2p);				
		// Le envio al Listener el socket p2p (nueva conexion)
		Event evListener;
		evListener.id = LS_ADD_SOCKET_P2P;
		evListener.tag = Tools::duplicate (socketBuffer);
		Listener::instance()->post_event(evListener, true);			

		sprintf(logBuffer, "Esperando respuesta del vendedor [%s]...", vendedor.c_str());		
		Tools::info(logBuffer);		
		sleep(DEFAULT_WAIT_SECONDS+1); // Solo para darle tiempo a que el listener, incorpore en su set de socket de escucha al socket p2p
		
		send_to_socket_message(sock_p2p, mensaje_xml);
	}		
	else
	{
		sprintf(logBuffer, "Router: start_p2p_connect: No se logro la comunicacion p2p con el vendedor [%s]", vendedor.c_str());		
		Tools::error(logBuffer);	
	}
}


void Router::close_p2p_connect(const void* msg)
{
	char logBuffer[BUFFER_SIZE];
	//Mensaje *mensaje = (Mensaje *)msg;
	//std::string mensaje_xml = mensaje->to_string();
	//sprintf(logBuffer, "Router: close_p2p_connect: Se va a enviar el xml [%s]", mensaje_xml.c_str());		
	//Tools::debug(logBuffer);
	
	//send_to_socket_message(sock_p2p, mensaje_xml);
	
	//sprintf(logBuffer, "Esperando que le llegue la respuesta al vendedor [%s], para asi cerrar el socket p2p...", mensaje->get_vendedor().c_str());
	//Tools::info(logBuffer);
	//sleep(DEFAULT_WAIT_SECONDS+1);
	
	sprintf(logBuffer, "Cerrando socket p2p [%d]", sock_p2p);		
	Tools::info(logBuffer);
	
	close(sock_p2p);
	sock_p2p = SOCK_ERRONEO;
}

void Router::send_to_next_node_message(const void* msg)
{
	char logBuffer[BUFFER_SIZE];
	Mensaje *mensaje = (Mensaje *)msg;	
	std::string next_node_name = mensaje->get_next_node_name();
	sprintf(logBuffer, "Router: send_to_next_node_message: Se obtuvo de mensaje->get_next_node_name el nombre [%s]", next_node_name.c_str());		
	Tools::debug(logBuffer);	
	
	std::string mensaje_xml = mensaje->to_string();
	sprintf(logBuffer, "Router: send_to_next_node_message: Se va a enviar el xml [%s]", mensaje_xml.c_str());		
	Tools::debug(logBuffer);
	
	int socket = nodo_socket_servidor_map[next_node_name];	
	sprintf(logBuffer, "Router: send_to_next_node_message: Se obtuvo de nodo_socket_map[%s] el socket [%d]", next_node_name.c_str(), socket);		
	Tools::debug(logBuffer);	
	send_to_socket_message(socket, mensaje_xml);
}


void Router::send_to_p2p_socket_message(const void* msg)
{
	char logBuffer[BUFFER_SIZE];
	Mensaje *mensaje = (Mensaje *)msg;	
	
	std::string mensaje_xml = mensaje->to_string();
	sprintf(logBuffer, "Router: send_to_next_node_message: Se va a enviar el xml [%s]", mensaje_xml.c_str());		
	Tools::debug(logBuffer);
	
	send_to_socket_message(sock_p2p, mensaje_xml);
}


void Router::send_to_socket_message(int socket, std::string mensaje_xml)
{
	char logBuffer[BUFFER_SIZE];
	
	if (socket != SOCK_ERRONEO)
	{		
		SocketUtil::enviar_mensaje(socket, mensaje_xml);	
		sprintf(logBuffer, "Router: send_to_socket_message: Se envio el mensaje solo al socket [%d]", socket);		
		Tools::debug(logBuffer);
	}
	else
	{
		Tools::error("Router: send_to_socket_message: el socket es erroneo");
		if (mensaje_xml.empty())
		{
			Tools::error("Router: send_to_socket_message: el mensaje es vacio");	
		}
	}
}



char *Router::get_handshake_msg()
{
	char logBuffer[BUFFER_SIZE];
	//memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Enviando handshake [%s]", nombre_nodo);
	Tools::info(logBuffer);			
	return nombre_nodo;
}

void Router::decode_rta_handshake_msg(const char *msg)
{
	char logBuffer[BUFFER_SIZE];
	if (msg != NULL)
	{
		sprintf(logBuffer, "Recibiendo respuesta handshake [%s]", msg);		
		Tools::info(logBuffer);
	}
	else
	{
		Tools::error("Listener: decode_rta_handshake_msg: Mensaje NULL");
	}
}



/************************************************************/
/*************** RUTINAS SOBRE LOS MAPAS ********************/
/************************************************************/
std::string Router::find_in_client_sockets_nodo_nombre(int id_socket)
{	
	char logBuffer[BUFFER_SIZE];
	memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Router: find_in_client_sockets_nodo_nombre: id_socket [%d]", id_socket);
	Tools::debug(logBuffer);			
	bool encontrado = false;
	int socket_aux = SOCK_ERRONEO;
	std::string nombre_nodo;
	for (NodoMappingIterator it = nodo_socket_cliente_map.begin(); it != nodo_socket_cliente_map.end(); it++)
	{
		socket_aux = (*it).second;
		memset(logBuffer, 0 , sizeof(logBuffer));
		sprintf(logBuffer, "Router: find_in_client_sockets_nodo_nombre: Id Socket actual [%d]", socket_aux);
		Tools::debug(logBuffer);			
		if (socket_aux == id_socket)
		{
			nombre_nodo = Tools::duplicate(it->first);			
			encontrado = true;				
			//memset(logBuffer, 0 , sizeof(logBuffer));
			sprintf(logBuffer, "Router: find_in_client_sockets_nodo_nombre: nombre_nodo [%s]", nombre_nodo.c_str());
			Tools::debug(logBuffer);				
		}
	}
	if (!encontrado)
	{
		sprintf(logBuffer, "Router: find_in_client_sockets_nodo_nombre: el id_socket [%d] no existe", id_socket);
		Tools::error(logBuffer);
	}
	return nombre_nodo;
}

std::string Router::find_in_server_sockets_nodo_nombre(int id_socket)
{	
	char logBuffer[BUFFER_SIZE];
	memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Router: find_in_server_sockets_nodo_nombre: id_socket [%d]", id_socket);
	Tools::debug(logBuffer);			
	bool encontrado = false;
	int socket_aux = SOCK_ERRONEO;
	std::string nombre_nodo;
	for (NodoMappingIterator it = nodo_socket_servidor_map.begin(); it != nodo_socket_servidor_map.end(); it++)
	{
		socket_aux = (*it).second;
		memset(logBuffer, 0 , sizeof(logBuffer));
		sprintf(logBuffer, "Router: find_in_server_sockets_nodo_nombre: Id Socket actual [%d]", socket_aux);
		Tools::debug(logBuffer);			
		if (socket_aux == id_socket)
		{
			nombre_nodo = Tools::duplicate(it->first);			
			encontrado = true;				
			//memset(logBuffer, 0 , sizeof(logBuffer));
			sprintf(logBuffer, "Router: find_in_server_sockets_nodo_nombre: nombre_nodo [%s]", nombre_nodo.c_str());
			Tools::debug(logBuffer);				
		}
	}
	if (!encontrado)
	{
		sprintf(logBuffer, "Router: find_in_server_sockets_nodo_nombre: el id_socket [%d] no existe", id_socket);
		Tools::error(logBuffer);
	}
	return nombre_nodo;
}

void Router::add_nodo_socket_into_server_map(std::string nombre_nodo, int id_socket)
{
	char logBuffer[BUFFER_SIZE];
	if (!nombre_nodo.empty())
	{
		nodo_socket_servidor_map[nombre_nodo] = id_socket;
		//memset(logBuffer, 0 , sizeof(logBuffer));
		sprintf(logBuffer, "Router: add_nodo_socket_into_server_map: Se agrego el par [%s,%d]", nombre_nodo.c_str(), id_socket);
		Tools::debug(logBuffer);
	}
	else
	{
		Tools::error("Router: add_nodo_socket_into_server_map: nombre_nodo es vacio");		
	}
}

void Router::add_nodo_socket_into_client_map(std::string nombre_nodo, int id_socket)
{
	char logBuffer[BUFFER_SIZE];
	if (!nombre_nodo.empty())
	{
		nodo_socket_cliente_map[nombre_nodo] = id_socket;
		//memset(logBuffer, 0 , sizeof(logBuffer));
		sprintf(logBuffer, "Router: add_nodo_socket_into_client_map: Se agrego el par [%s,%d]", nombre_nodo.c_str(), id_socket);
		Tools::debug(logBuffer);
	}
	else
	{
		Tools::error("Router: add_nodo_socket_into_client_map: nombre_nodo es vacio");		
	}
}

void Router::rm_nodo_socket_from_server_map(std::string nombre_nodo)
{
	char logBuffer[BUFFER_SIZE];

	if (!nombre_nodo.empty())
	{
		nodo_socket_servidor_map.erase(nombre_nodo);
		//memset(logBuffer, 0 , sizeof(logBuffer));
		sprintf(logBuffer, "Router: rm_nodo_socket_from_server_map: Se elimino nodo_socket_map.erase(%s)", nombre_nodo.c_str());
		Tools::debug(logBuffer);	
		sprintf(logBuffer, "Se retiro el nodo [%s]", nombre_nodo.c_str());
		Tools::info(logBuffer);			
	}
	else
	{
		Tools::error("Router: rm_nodo_socket_from_server_map: nombre_nodo es vacio");		
	}		
}

void Router::rm_nodo_socket_from_client_map(std::string nombre_nodo)
{
	char logBuffer[BUFFER_SIZE];

	if (!nombre_nodo.empty())
	{
		nodo_socket_cliente_map.erase(nombre_nodo);
		//memset(logBuffer, 0 , sizeof(logBuffer));
		sprintf(logBuffer, "Router: rm_nodo_socket_from_client_map: Se elimino nodo_socket_map.erase(%s)", nombre_nodo.c_str());
		Tools::debug(logBuffer);	
		
		sprintf(logBuffer, "Se retiro el nodo [%s]", nombre_nodo.c_str());
		Tools::info(logBuffer);		
	}
	else
	{
		Tools::error("Router: rm_nodo_socket_from_client_map: nombre_nodo es vacio");		
	}		
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



