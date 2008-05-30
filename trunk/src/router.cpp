

#include "common/tools.h"
#include "common/types.h"
#include "common/socket_util.h"
#include "router.h"
#include <iostream>
#include <assert.h>
#include <unistd.h>

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
			
		case LOOKUP:
			Tools::debug("Router: on_event: LOOKUP:");
			//procesar_look_up(ev.tag);
			break;
			
		case REPLAY:
			Tools::debug("Router: on_event: REPLAY:");
			break;
						
		case BUY:
			Tools::debug("Router: on_event: BUY:");
			//procesar_buy(ev.tag);			
			break;	
			
		case PRE_QUIT:
			Tools::debug("Router: on_event: QUIT:");
			procesar_pre_quit(ev.tag);
			break;	
			
		default:
			Tools::debug("Router: on event: *UNKNOWN*.");
			break;
	}
}

void Router::procesar_start_connection(const void *event_tag) 
{	
	char echoBuffer[BUFFER_SIZE];		// Buffer for echo string 
    int bytesRcvd;						// Bytes read in single recv() 	
	
	Tools* tools = Tools::instance();
	//tools->Config_Parser("lista_participante.conf");
	Tools::debug (nombre_nodo);
	ConfigDS* my_config = tools->get_info_nodo(nombre_nodo);
	//printf("my_config: nombre: [%s], ip: [%s], puerto: [%d]\n", my_config->nombre, my_config->ip, my_config->port);		
	ConfigDS* vecino1_config = tools->get_info_nodo(my_config->vecino1);	
	//printf("vecino1_config: nombre: [%s], ip: [%s], puerto: [%d]\n", vecino1_config->nombre, vecino1_config->ip, vecino1_config->port);
	if (vecino1_config == NULL) 
	{
		printf("El nodo %s, no existe en el archivo de configuracion\n", my_config->vecino1);
		exit(1);
	}
	
	ConfigDS* vecino2_config = tools->get_info_nodo(my_config->vecino2);
	if (vecino2_config == NULL) 
	{
		printf("El nombre de nodo %s, no existe en el archivo de configuracion\n", my_config->vecino2);
		exit(1);
	}
	
	sock_vecino1 = SocketUtil::cliente_abrir_conexion_tcp (vecino1_config->ip, vecino1_config->port);
	if (sock_vecino1 != SOCK_ERRONEO)
	{	
		memset(echoBuffer, 0, sizeof(echoBuffer));
		if ((bytesRcvd = recv(sock_vecino1, echoBuffer, BUFFER_SIZE, 0)) < 0)
		{
			perror("recv() failed or connection closed prematurely: vecino1");
			close(sock_vecino1);
			exit(1);
		}
		decode_mesage(echoBuffer);
	}
	
	sock_vecino2 = SocketUtil::cliente_abrir_conexion_tcp (vecino2_config->ip, vecino2_config->port);	
	if (sock_vecino2 != SOCK_ERRONEO)
	{		
		memset(echoBuffer, 0, sizeof(echoBuffer));
		if ((bytesRcvd = recv(sock_vecino2, echoBuffer, BUFFER_SIZE, 0)) < 0)
		{
			perror("recv() failed or connection closed prematurely: vecino2\n");
			close(sock_vecino2);
			exit(1);
		}
		decode_mesage(echoBuffer);
	}
	
	if (intentos_reconexion > 0 && (sock_vecino1 == SOCK_ERRONEO || sock_vecino1 == SOCK_ERRONEO)) 
	{
		printf("No se pudo lograr la comunicacion con al menos uno de los dos vecinos\n");
		printf("Intengo de reconexion nro %d. Se espera un delay de %d\n", intentos_reconexion, delay_reconexion);
		intentos_reconexion--;
		sleep(delay_reconexion);
		
		Event ev;
		ev.id = INIT;		
		this->post_event (ev, true);
	}
	else 	// Cuando se agotan los intentos de reconexion o ambos sockets hayan logrado conectarse	
	{
		// Comenzar a Comprar / Vender
		//Event ev;
		//ev.id = INIT;		
		//this->post_event (ev, true);
	}
}


void Router::decode_mesage(char* buffer)
{
	//<event client="player0" timestamp="12341234" />
	//using namespace xmlpp;
	printf("buffer: {%s}\n", buffer);
	/*Tools::debug("> Router::decode_mesage");		
	time_t last_tms = 0;
		
	DomParser parser;
	parser.parse_memory(buffer);
	
	Document* doc = parser.get_document();
	Element* root = doc->get_root_node();
	assert(root);
	
	NodeSet nodes = root->find("/event");
	assert(!nodes.empty());
	Node* node = nodes[0];

	Element* elem = dynamic_cast<Element*>(node);
			
	std::string node_name = elem->get_name().c_str();
	elem->get_child_text();
	
	Attribute* attr = NULL;
	attr = elem->get_attribute("client");
	const char * client = attr->get_value().c_str();
	
	Tools::debug("El connect retorno el siguiente client");
	Tools::debug(client);	
	attr = elem->get_attribute("timestamp");
	time_t tms = atoi(attr->get_value().c_str());
	
	//Sender::instance()->playerId = client;
	//Receptor::instance()->tms = tms;
	Tools::debug("< Router: decode_mesage");*/
}

void Router::procesar_pre_quit(const void *event_tag) 
{
	// Inicia la rutina de salida
	close(sock_vecino1);
	close(sock_vecino2);
	
	//Event ev;
	//ev.id  = QUIT;
	//this->post_event(ev, true);
}



