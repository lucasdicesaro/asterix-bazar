

#include "common/tools.h"
#include "common/types.h"
#include "logic.h"
#include "router.h"
#include <assert.h>

extern char *nombre_nodo;

Logic* Logic::single_instance = NULL;
/**
* Constructors
*/
Logic::Logic()
{
	ReconnectParamsDS *reconnectParams = Tools::instance()->get_reconnect_params();
	hopcount = reconnectParams->hopcount;
}

Logic::~Logic()
{
}

Logic* Logic::instance()
{
	if (!single_instance)
		single_instance = new Logic;
	return single_instance;
}

void Logic::on_event(const Event& ev)
{
	Event evBroad;
	Mensaje *mensaje = NULL;
	std::string vendedor;
	switch (ev.id)
	{
		case INIT:
			Tools::debug("Logic: on_event: INIT:");
			break;
			
		case DO_LOOKUP:
			Tools::debug("Logic: on_event: DO_LOOKUP:");
			// TODO definir los criterios del producto a elegir para comprar y la cantidad
			mensaje = build_look_up_msg(PRODUCTO_SAL, 2);
			add_nodo(mensaje, nombre_nodo);
			
			evBroad.id = BROADCAST;
			evBroad.tag = Tools::duplicate(mensaje->to_string());
			Router::instance()->post_event(evBroad, true);			
			break;
			
		case DO_REPLY:
			Tools::debug("Logic: on_event: DO_REPLY:");
			// TODO definir 
			mensaje = build_reply_msg();
			//add_nodo(mensaje, nombre_nodo);			
			
			evBroad.id = BROADCAST;
			evBroad.tag = Tools::duplicate(mensaje->to_string());
			Router::instance()->post_event(evBroad, true);			
			break;
			
		case DO_BUY:
			Tools::debug("Logic: on_event: DO_BUY:");
			// TODO definir 
			vendedor = (char *)ev.tag;
			mensaje = build_buy_msg(vendedor);
			//add_nodo(mensaje, nombre_nodo);
			
			evBroad.id = BROADCAST;
			evBroad.tag = Tools::duplicate(mensaje->to_string());
			Router::instance()->post_event(evBroad, true);			
			break;			
			
		case CLIENT_MSG:
			Tools::debug("Logic: on_event: CLIENT_MSG:");
			on_client_msg(ev.tag);
			break;			
						
		default:
			Tools::debug("Logic: on event: *UNKNOWN*.");
			break;
	}
}




Mensaje *Logic::build_look_up_msg(std::string product_name, int cantidad)
{
	Mensaje *mensaje = new Mensaje();
	mensaje->set_code(CODIGO_LOOKUP);
	mensaje->set_product_name(product_name);
	mensaje->set_cantidad(cantidad);
	mensaje->set_hopcount(hopcount);
	return mensaje;
}

Mensaje *Logic::build_reply_msg()
{
	Mensaje *mensaje = new Mensaje();
	mensaje->set_code(CODIGO_REPLY);
	mensaje->set_vendedor(nombre_nodo);
	mensaje->set_cantidad(-1);
	mensaje->set_hopcount(-1);
	return mensaje;
}

Mensaje *Logic::build_buy_msg(std::string vendedor)
{
	Mensaje *mensaje = new Mensaje();
	mensaje->set_code(CODIGO_BUY);
	mensaje->set_vendedor(vendedor);
	mensaje->set_cantidad(-1);
	mensaje->set_hopcount(-1);
	return mensaje;
}


/**
Locked Add Nodo.
*/
void Logic::add_nodo(Mensaje *mensaje, const char*buffer)
{
	lock();
	Nodo* nodo = new Nodo;
	nodo->name = Tools::duplicate(buffer);
	//clients[nodo->name] = nodo;
	mensaje->get_nodos().push_back(nodo);
	unlock();
}


/**
Client msg handler:
*/
void Logic::on_client_msg(const void* xml_tag)
{	
	// Verificar si tengo lo que pide, si es Look up
	// Verificar si el paquete es mio, si es un Replay
	// Analizar todos lo casos..
	// Si no me corresponde
	// Decrementar el hopcount
	// y enviar a mis vecinos
		
	assert(xml_tag);
	std::string xml = (char *)xml_tag;
	Mensaje *mensaje = new Mensaje();
	mensaje->load(xml);
	
	
	char logBuffer[BUFFER_SIZE];
	memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Logic: Mensaje recibido [%s]", xml.c_str());
	Tools::debug(logBuffer);
	
	//memset(logBuffer, 0 , sizeof(logBuffer));
	//sprintf(logBuffer, "Logic: mensaje->code [%s]", mensaje->get_code().c_str());
	//Tools::debug(logBuffer);
	
	if (mensaje->get_code().compare(CODIGO_LOOKUP) == 0) 
	{
		memset(logBuffer, 0 , sizeof(logBuffer));
		sprintf(logBuffer, "Logic: Es un lookup. Tengo [%d] unidades de [%s]?", 
				mensaje->get_cantidad(), mensaje->get_product_name().c_str());
		Tools::debug(logBuffer);
		
		Event ev;
		ev.id = DO_REPLY;
		this->post_event(ev, true);
	}
	else if (mensaje->get_code().compare(CODIGO_REPLY) == 0)
	{
		Tools::debug("Logic: codigo REPLY:");
		
		Event ev;
		ev.id = DO_BUY;
		ev.tag = Tools::duplicate(mensaje->get_vendedor());
		this->post_event(ev, true);
	}
	else if (mensaje->get_code().compare(CODIGO_BUY) == 0)
	{
		Tools::debug("Logic: codigo BUY:");
		// Preparar todo para recibir un connect del comprador.
	}
}


/**
Getters:
*/
//Mensaje& Logic::get_mensaje()
//{
//	return mensaje;
//}



