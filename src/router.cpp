

#include "common/tools.h"
#include "common/types.h"
#include "common/socket_util.h"
#include "router.h"
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <signal.h>

Router* Router::single_instance = NULL;

extern char *nombre_nodo;
int intentos_reconexion = INTENTOS_RECONEXION;
int delay_reconexion = DELAY_RECONEXION;

/**
* Constructors
*/
Router::Router()
{
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
			procesar_start_connection(ev.tag);
			break;
			
		case SEND_MSG:
			Tools::debug("Router: on_event: SEND_MSG:");
			send_message(ev.tag);
			break;	
			
		default:
			Tools::debug("Router: on event: *UNKNOWN*.");
			break;
	}
}

void Router::procesar_start_connection(const void *event_tag) 
{	
	char logBuffer[BUFFER_SIZE];		// Buffer for log
	char *echoBuffer = new char[BUFFER_SIZE];		// Buffer for echo string     
	
	Tools* tools = Tools::instance();
	//tools->Config_Parser("lista_participante.conf");
	//Tools::debug (nombre_nodo);
	ConfigDS* my_config = tools->get_info_nodo(nombre_nodo);
	//printf("my_config: nombre: [%s], ip: [%s], puerto: [%d]\n", my_config->nombre, my_config->ip, my_config->port);		
	ConfigDS* vecino1_config = tools->get_info_nodo(my_config->vecino1);	
	//printf("vecino1_config: nombre: [%s], ip: [%s], puerto: [%d]\n", vecino1_config->nombre, vecino1_config->ip, vecino1_config->port);
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
		echoBuffer = SocketUtil::recibir_mensaje(sock_vecino1);
		decode_mesage(echoBuffer);
	}
	
	sock_vecino2 = SocketUtil::cliente_abrir_conexion_tcp (vecino2_config->ip, vecino2_config->port);	
	if (sock_vecino2 != SOCK_ERRONEO)
	{		
		echoBuffer = SocketUtil::recibir_mensaje(sock_vecino2);
		decode_mesage(echoBuffer);
	}
	
	bool conexiones_incompletas = (sock_vecino1 == SOCK_ERRONEO || sock_vecino1 == SOCK_ERRONEO);
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
			Tools::info("Router: Se agotaron los intentos de reconexion");		
		}
		if (!conexiones_incompletas) 
		{
			Tools::info("Router: Se logro completar las conexiones");
		}
		// Comenzar a Comprar / Vender
		//Event ev;
		//ev.id = INIT;		
		//this->post_event (ev, true);
	}
}


void Router::decode_mesage(char* buffer)
{
	char logBuffer[BUFFER_SIZE];		// Buffer for log
	//<event ticket="socket_id" timestamp="12341234" />
	using namespace xmlpp;
	sprintf(logBuffer, "Router: Decodificando handshake del server [%s]", buffer);
	Tools::info(logBuffer);
	
	//time_t last_tms = 0;
		
	DomParser parser;
	parser.parse_memory(buffer);
	
	Document* doc = parser.get_document();
	Element* root = doc->get_root_node();
	assert(root);
	
	std::string bar("/");
	std::string root_element(XML_WELCOME_ROOT_ELEMENT);
	NodeSet nodes = root->find(bar + root_element);
	//NodeSet nodes = root->find("/event");
	assert(!nodes.empty());
	Node* node = nodes[0];

	Element* elem = dynamic_cast<Element*>(node);
			
	std::string node_name = elem->get_name().c_str();
	elem->get_child_text();
	
	Attribute* attr = NULL;
	//attr = elem->get_attribute("client");
	attr = elem->get_attribute(XML_WELCOME_FIRST_ELEMENT);
	const char * sockectId = attr->get_value().c_str();	
	std::string sockectId_str = sockectId;
	Tools::info("Router: El connect retorno [" + sockectId_str + "]");
	
	//attr = elem->get_attribute("timestamp");
	//time_t tms = atoi(attr->get_value().c_str());
	
	//Sender::instance()->playerId = client;
	//Receptor::instance()->tms = tms;
	//Tools::debug("< Router: decode_mesage");
}


void Router::send_message(const void* msg)
{
	char logBuffer[BUFFER_SIZE];		// Buffer for log
	const char * mensaje = (const char*)msg;
	
	sprintf(logBuffer, "Router: Se va a enviar el mensaje [%s]", mensaje);
	Tools::info(logBuffer);	

	// TODO Hay que saber a que VECINO se lo mando 

	unsigned int mensajeLen = strlen(mensaje)+1;
	if (send(sock_vecino1, mensaje, mensajeLen, 0) != mensajeLen)
	{
		Tools::error("send() sent a different number of bytes than expected");
		throw "";
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



