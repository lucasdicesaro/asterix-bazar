

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
	Event evRta;
	Mensaje *mensaje = NULL;
	std::string vendedor;
	char logBuffer[BUFFER_SIZE];
	
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
			Tools::debug("Logic: on_event: Se agrego NODO");
			memset(logBuffer, 0 , sizeof(logBuffer));
			sprintf(logBuffer, "Logic: on_event: Se va a enviar el mensaje [%s]", mensaje->to_string().c_str());
			Tools::debug(logBuffer);	
			
			evRta.id = BROADCAST;
			evRta.tag = Tools::duplicate(mensaje->to_string());
			Router::instance()->post_event(evRta, true);			
			break;
			
		case DO_LOOKUP_FORWARD:
			Tools::debug("Logic: on_event: DO_LOOKUP_FORWARD:");
			// TODO  Decrementar el hopcount
			// y enviar a mis vecinos

			mensaje = (Mensaje *)ev.tag;
			if (mensaje != NULL)
			{				
				less_hopcount(mensaje);
				if (mensaje->get_hopcount() > 0)				
				{					
					add_nodo(mensaje, nombre_nodo);					
					Tools::debug("Logic: on_event: Reenviando el LOOKUP");					
					evRta.id = BROADCAST;
					evRta.tag = Tools::duplicate(mensaje->to_string());
					Router::instance()->post_event(evRta, true);					
				}
				else 
				{
					//Tools::debug("Logic: on_event: Se descarto el mensaje, por agotarse su hopcount");
					
					
					
				// BORRAR !!! ESTO !!! QUE ES SOLO PARA PRUEBA!!!
				Tools::debug("Logic: on_event: SE INICIA LA RESPUESTA");
				evRta.id = DO_REPLY;
				evRta.tag = mensaje;
				this->post_event(evRta, true);
				// BORRAR !!! ESTO !!! QUE ES SOLO PARA PRUEBA!!!

					
					
				}	
			}
			else
			{
				Tools::error("Logic: on_event: mensaje es NULL en DO_LOOKUP_FORWARD");
			}
			break;								
			
		case DO_REPLY:
			Tools::debug("Logic: on_event: DO_REPLY:"); 
			mensaje = (Mensaje *)ev.tag;
			mensaje = build_reply_msg(mensaje);
			if (mensaje != NULL)
			{
				Tools::debug("Logic: on_event: Enviando el REPLY");
				// TODO se borra el nodo la 1era vez ?
				//less_nodo(mensaje);				
				evRta.id = SEND_TO_SOCKET;
				evRta.tag = mensaje;
				Router::instance()->post_event(evRta, true);					
			}
			else
			{
				Tools::error("Logic: on_event: mensaje es NULL en DO_REPLY_FORWARD");
			}			
			break;
						
		case DO_REPLY_FORWARD:
			Tools::debug("Logic: on_event: DO_REPLY_FORWARD:");

			mensaje = (Mensaje *)ev.tag;
			if (mensaje != NULL)
			{
				Tools::debug("Logic: on_event: Reenviando el REPLY");
				less_nodo(mensaje);				
				evRta.id = SEND_TO_SOCKET;
				evRta.tag = mensaje;
				Router::instance()->post_event(evRta, true);					
			}
			else
			{
				Tools::error("Logic: on_event: mensaje es NULL en DO_REPLY_FORWARD");
			}
			break;			
			
		case DO_BUY:
			Tools::debug("Logic: on_event: DO_BUY:");
			vendedor = (char *)ev.tag;
			mensaje = build_buy_msg(vendedor);
			// TODO Preparar todo para iniciar un connect al vendedor
			evRta.id = BROADCAST;
			evRta.tag = Tools::duplicate(mensaje->to_string());
			Router::instance()->post_event(evRta, true);			
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
	Tools::debug("Logic: build_look_up_msg:");
	Mensaje *mensaje = new Mensaje();
	mensaje->set_code(CODIGO_LOOKUP);
	mensaje->set_product_name(product_name);
	mensaje->set_cantidad(cantidad);
	mensaje->set_hopcount(HOPCOUNT);
	return mensaje;
}

Mensaje *Logic::build_reply_msg(Mensaje *mensaje)
{
	//Mensaje *mensaje = new Mensaje();
	Tools::debug("Logic: build_reply_msg:");
	if (mensaje != NULL)
	{
		mensaje->set_code(CODIGO_REPLY);
		mensaje->set_vendedor(nombre_nodo);
		mensaje->set_cantidad(-1);
		mensaje->set_hopcount(-1);
	}
	else
	{
		Tools::error ("Logic: build_reply_msg: Mensaje es NULO");
	}
	return mensaje;
}

Mensaje *Logic::build_buy_msg(std::string vendedor)
{
	Tools::debug("Logic: build_buy_msg:");
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
	Tools::debug("Logic: add_nodo:");
	//lock();
	Nodo* nodo = new Nodo;
	nodo->name = Tools::duplicate(buffer);
	//clients[nodo->name] = nodo;
	mensaje->get_nodos().push_back(nodo);
	//unlock();
}

void Logic::less_nodo(Mensaje *mensaje)
{
//http://www.cplusplus.com/reference/stl/list/
	Tools::debug("Logic: less_nodo:");
	//lock();
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
	//unlock();
}

void Logic::less_hopcount(Mensaje *mensaje)
{
	Tools::debug("Logic: less_hopcount:");
	//lock();
	mensaje->set_hopcount(mensaje->get_hopcount()-1);
	//unlock();
}

std::string Logic::get_next_node_name(const void* msg)
{
	Mensaje *mensaje = (Mensaje*)msg;
	return get_next_node_name(mensaje);
}

std::string Logic::get_next_node_name(Mensaje *mensaje)
{
	char logBuffer[BUFFER_SIZE];
	memset(logBuffer, 0 , sizeof(logBuffer));
	//Mensaje *mensaje = (Mensaje*)msg;	
	Nodo *nodo = NULL;
	std::string node_name;
	if (!mensaje->get_nodos().empty())
	{
		nodo = (Nodo *)mensaje->get_nodos().back();		
		if (nodo != NULL)
		{
			node_name = Tools::duplicate(nodo->name);
			sprintf(logBuffer, "Logic: get_next_node_name: nombre de nodo a retornar [%s]", node_name.c_str());
			Tools::debug(logBuffer);			
		}
		else
		{
			Tools::debug("Logic: less_nodo: nodo es NULL");
		}		
	}
	else
	{
		Tools::debug("Logic: less_nodo: mensaje->get_nodos() esta vacia");
	}
	return node_name;
}

std::string Logic::get_mensaje_as_string(const void* msg)
{
	Mensaje *mensaje = (Mensaje*)msg;	
	std::string mensaje_as_string;
	if (mensaje != NULL)
	{
		mensaje_as_string = Tools::duplicate(mensaje->to_string());
	}
	else
	{
		Tools::error ("Logic: get_mensaje_as_string: El mensaje es NULL");
	}
	return mensaje_as_string;
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
	sprintf(logBuffer, "Logic: on_client_msg: Mensaje recibido [%s]", xml.c_str());
	Tools::debug(logBuffer);
	
	//memset(logBuffer, 0 , sizeof(logBuffer));
	//sprintf(logBuffer, "Logic: mensaje->code [%s]", mensaje->get_code().c_str());
	//Tools::debug(logBuffer);
	
	if (mensaje->get_code().compare(CODIGO_LOOKUP) == 0) 
	{
		memset(logBuffer, 0 , sizeof(logBuffer));
		sprintf(logBuffer, "Logic: on_client_msg: Es un lookup. Tengo [%d] unidades de [%s]?", 
				mensaje->get_cantidad(), mensaje->get_product_name().c_str());
		Tools::debug(logBuffer);
		
		// TODO Determinar las condiciones que dicen si tengo o no el producto y la cantidad
		bool tengoProductoCantidad = false;
		Event ev;
		ev.tag = mensaje;
		if (tengoProductoCantidad)
		{			
			ev.id = DO_REPLY;
		}
		else
		{
			ev.id = DO_LOOKUP_FORWARD;					
		}
		this->post_event(ev, true);
	}
	else if (mensaje->get_code().compare(CODIGO_REPLY) == 0)
	{
		Tools::debug("Logic: on_client_msg: codigo REPLY:");
		
		// TODO Determinar las condiciones que dicen si lo que me llega como REPLY
		// es a partir de un LOOKUP mio o si lo tengo que seguir pasando por donde vino
		// Supongo que tengo que guardar una lista de ids de los paquetes que envio 
		// o bien, si queda un unico nodo y soy yo 
		
		// El paquete debe volver por el camino por donde fue.
		// Si todo esta bien, cuando queda un nodo solo en el mensaje, 
		// el nombre de ese nodo, tiene que coincidir con el nombre de nodo que lo inicio.
		Event ev;		
		std::string last_nodo_name;
		if (mensaje->count() <= 1)
		{
			last_nodo_name = get_next_node_name(mensaje);
			memset(logBuffer, 0 , sizeof(logBuffer));
			sprintf(logBuffer, "Logic: on_client_msg: Queda solo un nodo en el mensaje [%s]", last_nodo_name.c_str());
			Tools::debug(logBuffer);			
			ev.id = DO_BUY;
			ev.tag = Tools::duplicate(mensaje->get_vendedor());			
		}
		else
		{
			Tools::debug("Logic: on_client_msg: Sigue quedando mas de un nodo en el mensaje. Se reenvia");
			ev.id = DO_REPLY_FORWARD;
			ev.tag = mensaje;
		}
		this->post_event(ev, true);
	}
	else if (mensaje->get_code().compare(CODIGO_BUY) == 0)
	{
		Tools::debug("Logic: on_client_msg: codigo BUY:");
		// TODO Preparar todo para recibir un connect del comprador.
		// Cómo sé la cantidad de producto que "vendo"/"quiere el cliente"?
		// Supongo que hay que mantener una lista de los lookups que respondi (yo, como vendedor)
	}
}


/**
Getters:
*/
//Mensaje& Logic::get_mensaje()
//{
//	return mensaje;
//}



