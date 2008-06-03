

#include "common/tools.h"
#include "common/types.h"
#include "logic.h"
#include "router.h"
#include <assert.h>

extern char *nombre_nodo;

Logic* Logic::single_instance = NULL;
int Logic::HOPCOUNT;
/**
* Constructors
*/
Logic::Logic()
{
	ReconnectParamsDS *reconnectParams = Tools::instance()->get_reconnect_params();
	Logic::HOPCOUNT = reconnectParams->hopcount;
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
			
		case DO_LOOKUP_FORWARD:
			Tools::debug("Logic: on_event: DO_LOOKUP_FORWARD:");
			// TODO  Decrementar el hopcount
			// y enviar a mis vecinos

			mensaje = (Mensaje *)ev.tag;
			if (mensaje != NULL)
			{
				add_nodo(mensaje, nombre_nodo);
				less_hopcount(mensaje);
				if (mensaje->get_hopcount() > 0)				
				{					
					Tools::error("Logic: Reenviando el LOOKUP");					
					evBroad.id = BROADCAST;
					evBroad.tag = Tools::duplicate(mensaje->to_string());
					Router::instance()->post_event(evBroad, true);					
				}
				else 
				{
					Tools::debug("Logic: Se descarto el mensaje, por agotarse su hopcount");
				}	
			}
			else
			{
				Tools::error("Logic: mensaje es NULL en DO_LOOKUP_FORWARD");
			}
			break;								
			
		case DO_REPLY:
			Tools::debug("Logic: on_event: DO_REPLY:"); 
			mensaje = build_reply_msg();			
			
			evBroad.id = BROADCAST;
			evBroad.tag = Tools::duplicate(mensaje->to_string());
			Router::instance()->post_event(evBroad, true);			
			break;
						
		case DO_REPLY_FORWARD:
			Tools::debug("Logic: on_event: DO_REPLY_FORWARD:");

			mensaje = (Mensaje *)ev.tag;
			if (mensaje != NULL)
			{
				Tools::error("Logic: Reenviando el REPLY");
				less_nodo(mensaje);				
				// TODO 
				// Determinar si mando el mensaje->to_string() o el Mensaje, porque 
				// en el Router tengo que saber a quien (socket) se lo mando
				
				//evBroad.id = SEND_TO_SOCKET;
				//evBroad.tag = Tools::duplicate(mensaje->to_string());
				//Router::instance()->post_event(evBroad, true);					
			}
			else
			{
				Tools::error("Logic: mensaje es NULL en DO_REPLY_FORWARD");
			}
			break;			
			
		case DO_BUY:
			Tools::debug("Logic: on_event: DO_BUY:");
			vendedor = (char *)ev.tag;
			mensaje = build_buy_msg(vendedor);
			// TODO Preparar todo para iniciar un connect al vendedor
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
	mensaje->set_hopcount(HOPCOUNT);
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

void Logic::less_nodo(Mensaje *mensaje)
{
//http://www.cplusplus.com/reference/stl/list/
	lock();
	char logBuffer[BUFFER_SIZE];
	Nodo *nodo = (Nodo *)mensaje->get_nodos().back();
	memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Logic: less_nodo: Se elimino el nodo [%s]", nodo->name.c_str());
	Tools::debug(logBuffer);
	mensaje->get_nodos().pop_back();
	
	// TODO deberia borrar esto ?
	Tools::debug("Logic: less_nodo: Borrando nodo");
	delete nodo;	
	Tools::debug("Logic: less_nodo: Se Borro nodo");	
	unlock();
}

void Logic::less_hopcount(Mensaje *mensaje)
{
	lock();
	mensaje->set_hopcount(mensaje->get_hopcount()-1);
	unlock();
}



/**
Client msg handler:
*/
void Logic::on_client_msg(const void* xml_tag)
{		
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

		
		// TODO Determinar las condiciones que dicen si tengo o no el producto y 
		// la cantidad
		bool tengoProductoCantidad = false;
		Event ev;
		if (tengoProductoCantidad)
		{			
			ev.id = DO_REPLY;
		}
		else
		{
			ev.id = DO_LOOKUP_FORWARD;
			ev.tag = mensaje;			
		}
		this->post_event(ev, true);
	}
	else if (mensaje->get_code().compare(CODIGO_REPLY) == 0)
	{
		Tools::debug("Logic: codigo REPLY:");
		
		// TODO Determinar las condiciones que dicen si lo que me llega como REPLY
		// es mio o si lo tengo que seguir pasando por donde vino
		// Supongo que tengo que guardar una lista de los paquetes que envio
		// o bien si queda un unico nodo y soy yo 
		bool yoPediEsto = false;
		Event ev;
		if (yoPediEsto)
		{
			ev.id = DO_BUY;
			ev.tag = Tools::duplicate(mensaje->get_vendedor());			
		}
		else
		{
			ev.id = DO_REPLY_FORWARD;
			ev.tag = mensaje;
		}
		this->post_event(ev, true);		
	}
	else if (mensaje->get_code().compare(CODIGO_BUY) == 0)
	{
		Tools::debug("Logic: codigo BUY:");
		// TODO Preparar todo para recibir un connect del comprador.
		// Como se la cantidad del producto que vendo, quiere el cliente?
		// Supongo que hay que mantener una lista de los lookups que respondi (yo, vendedor)
	}
}


/**
Getters:
*/
//Mensaje& Logic::get_mensaje()
//{
//	return mensaje;
//}



