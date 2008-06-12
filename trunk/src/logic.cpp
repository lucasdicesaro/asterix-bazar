

#include "common/tools.h"
#include "common/types.h"
#include "logic.h"
#include "router.h"
#include "stock.h"
#include "common/tools.h"
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
	
	Stock* stock = Stock::instance();
	stock->load_stock();
	
	Tools* tools = Tools::instance();
	Tools::info_label_value("Logic: stock de sal:", stock->get_stock("sal"));
	Tools::info_label_value("Logic: stock de pescado:", stock->get_stock("pescado"));
	Tools::info_label_value("Logic: stock de verdura:", stock->get_stock("verdura"));
	std::string str_compro = stock->get_compro();
	std::string str_vendo = stock->get_vendo();
	Tools::info("Logic: compro: " + str_compro);
	Tools::info("Logic: vendo: " + str_vendo);
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
	char *buffer = new char[BUFFER_SIZE];
   switch (ev.id)
	{
		case INIT:
			Tools::debug("Logic: on_event: INIT:");
			break;
			
		case RUN_TIMEOUT:
			Tools::debug("Logic: on_event: RUN_TIMEOUT:");
			on_timeout();
			break;			
			
		case DO_LOOKUP:
			Tools::debug("Logic: on_event: DO_LOOKUP:");
			on_look_up();			
			break;
			
		case DO_LOOKUP_FORWARD:
			Tools::debug("Logic: on_event: DO_LOOKUP_FORWARD:");			
			on_look_up_forward(ev.tag);
			break;								
			
		case DO_REPLY:
			Tools::debug("Logic: on_event: DO_REPLY:"); 
			on_replay(ev.tag);
			break;
						
		case DO_REPLY_FORWARD:
			Tools::debug("Logic: on_event: DO_REPLY_FORWARD:");
			on_replay_forward(ev.tag);			
			break;			
			
		case DO_BUY:
			Tools::debug("Logic: on_event: DO_BUY:");
			on_buy(ev.tag);			
			break;			
			
		case CLIENT_MSG:
			Tools::debug("Logic: on_event: CLIENT_MSG:");
			on_client_msg(ev.tag);
			break;			

	   case TECLA:
			sprintf(buffer, "Logic: on_event: TECLA: %s", (char*) ev.tag);
   			Tools::debug(buffer);
			break;			

		  
		default:
			Tools::debug("Logic: on event: *UNKNOWN*.");
			break;
	}
}


void Logic::on_timeout()
{
	Tools::debug("Logic: on_timeout:");
	Event ev;
	ev.id = DO_LOOKUP;		
	this->post_event(ev, true);
}

void Logic::on_look_up()
{
	char logBuffer[BUFFER_SIZE];
	
	//obtiene producto a comprar de clase STOCK
	Stock* stock = Stock::instance();
	std::string str_compro = stock->get_compro();
	Mensaje *mensaje = build_look_up_msg(str_compro, 2);
	add_nodo(mensaje, nombre_nodo);
	
	//Tools::debug("Logic: on_event: Se agrego NODO");
	
	//memset(logBuffer, 0 , sizeof(logBuffer));
	//sprintf(logBuffer, "Logic: on_event: Se va a enviar el mensaje [%s]", mensaje->to_string().c_str());
	//Tools::debug(logBuffer);	
	
	Event evRta;
	evRta.id = FIRST_BROADCAST;
	evRta.tag = Tools::duplicate(mensaje->to_string());
	//evRta.tag = mensaje->clone();
	Router::instance()->post_event(evRta, true);	
}

void Logic::on_look_up_forward(const void* msg)
{
	Mensaje *mensaje = (Mensaje *)msg;
	
	if (mensaje != NULL)
	{				
		less_hopcount(mensaje);
		if (mensaje->get_hopcount() > 0)				
		{					
			add_nodo(mensaje, nombre_nodo);					
			Tools::debug("Logic: on_event: Reenviando el LOOKUP");					
			
			Event evRta;
			evRta.id = BROADCAST;
			//evRta.tag = Tools::duplicate(mensaje->to_string());
			evRta.tag = mensaje->clone();
			Router::instance()->post_event(evRta, true);					
		}
		else 
		{
			//Tools::debug("Logic: on_event: Se descarto el mensaje, por agotarse su hopcount");
			
			
			
		// BORRAR !!! ESTO !!! QUE ES SOLO PARA PRUEBA!!!
		Tools::debug("Logic: on_event: SE INICIA LA RESPUESTA");
		Event evRta;	
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
}

void Logic::on_replay(const void* msg)
{
	Mensaje *mensaje_old = (Mensaje *)msg;
	Mensaje *mensaje = build_reply_msg(mensaje_old);
	if (mensaje != NULL)
	{
		Tools::debug("Logic: on_event: Enviando el REPLY");
		// TODO se borra el nodo la 1era vez ?
		//less_nodo(mensaje);
		
		Event evRta;
		evRta.id = SEND_TO_SOCKET;
		evRta.tag = mensaje->clone();
		Router::instance()->post_event(evRta, true);					
	}
	else
	{
		Tools::error("Logic: on_event: mensaje es NULL en DO_REPLY_FORWARD. Operacion descatada");
	}		
}

void Logic::on_replay_forward(const void* msg)
{
	Mensaje *mensaje = (Mensaje *)msg;
	if (mensaje != NULL)
	{
		Tools::debug("Logic: on_event: Reenviando el REPLY");
		less_nodo(mensaje);		
		
		Event evRta;
		evRta.id = SEND_TO_SOCKET;
		evRta.tag = mensaje;
		Router::instance()->post_event(evRta, true);					
	}
	else
	{
		Tools::error("Logic: on_event: mensaje es NULL en DO_REPLY_FORWARD");
	}
}

void Logic::on_buy(const void* msg)
{
	char logBuffer[BUFFER_SIZE];
	std::string vendedor = (char *)msg;
	
	Mensaje *mensaje = build_buy_msg(vendedor, PRODUCTO_SAL, 2);
	// TODO Preparar todo para iniciar un connect al vendedor

	
	// SOLO ES PRUEBA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Aca termina el ciclo por ahora.... Asi que puedo iniciar una nueva compra en segundos...
	// Despues borrar esto. SOLO ES PRUEBA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	sleep(60);
	Event ev;
	ev.id = DO_LOOKUP;
	this->post_event(ev, true);	
		
		
	//Event evRta;
	//evRta.id = BROADCAST;
	//evRta.tag = Tools::duplicate(mensaje->to_string());
	//Router::instance()->post_event(evRta, true);
}

/**
 Client msg handlerd:
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
		on_codigo_look_up(mensaje);
	}
	else if (mensaje->get_code().compare(CODIGO_REPLY) == 0)
	{
		on_codigo_replay(mensaje);
	}
	else if (mensaje->get_code().compare(CODIGO_BUY) == 0)
	{
		on_codigo_buy(mensaje);
	}
}

void Logic::on_codigo_look_up(Mensaje *mensaje)
{
	Stock* stock = Stock::instance();
	bool tengoProductoCantidad = false;
		
	char logBuffer[BUFFER_SIZE];
	memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Logic: on_codigo_look_up: Tengo [%d] unidades de [%s]?", 
			mensaje->get_cantidad(), mensaje->get_product_name().c_str());
	Tools::debug(logBuffer);
	
	// Checkea stock para determinar si vendo lo solicitado y tengo stock
	
	std::string producto_solicitado = mensaje->get_product_name();
	int cantidad_solicitada = mensaje->get_cantidad();
	char* vendo = stock->get_vendo();
	if ((vendo == producto_solicitado) && (stock->get_stock(producto_solicitado) >= cantidad_solicitada)) 
	{
		Tools::debug("Logic: Vendo y tengo lo que pide");
		tengoProductoCantidad = true;
	}
	else
	{
		Tools::debug("Logic: NO tengo o vendo lo que pide");
		tengoProductoCantidad = false;
	}
	
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

void Logic::on_codigo_replay(Mensaje *mensaje)
{
	Tools::debug("Logic: on_codigo_replay:");
	char logBuffer[BUFFER_SIZE];
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
		last_nodo_name = mensaje->get_next_node_name();
		memset(logBuffer, 0 , sizeof(logBuffer));
		sprintf(logBuffer, "Logic: on_codigo_replay: Queda solo un nodo en el mensaje [%s]. Cant [%d]", last_nodo_name.c_str(), mensaje->count());
		Tools::debug(logBuffer);			
		
		if (last_nodo_name.compare(nombre_nodo) == 0)
		{
			Tools::debug("Logic: on_codigo_replay: El ultimo nodo contiene mi nombre");
			memset(logBuffer, 0 , sizeof(logBuffer));
			sprintf(logBuffer, "El vendedor [%s] vende lo que necesito: [%d] unidades de [%s]", mensaje->get_vendedor().c_str(), mensaje->get_cantidad(), mensaje->get_product_name().c_str());
			Tools::info(logBuffer);
			ev.id = DO_BUY;
			ev.tag = Tools::duplicate(mensaje->get_vendedor());			
		}
		else
		{
			Tools::warn("Logic: on_codigo_replay: El ultimo nodo No contiene mi nombre: No deberia llegar aqui. Descarto el mensaje");
		}
	}
	else
	{
		Tools::debug("Logic: on_codigo_replay: Sigue quedando mas de un nodo en el mensaje. Se reenvia");
		ev.id = DO_REPLY_FORWARD;
		ev.tag = mensaje;
	}
	this->post_event(ev, true);
}

void Logic::on_codigo_buy(Mensaje *mensaje)
{
	Tools::debug("Logic: on_codigo_buy:");
	// TODO Preparar todo para recibir un connect del comprador.
	// Cómo sé la cantidad de producto que "vendo"/"quiere el cliente"?
	// Supongo que hay que mantener una lista de los lookups que respondi (yo, como vendedor)	
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

Mensaje *Logic::build_reply_msg(Mensaje *mensaje_old)
{
	Mensaje *mensaje = NULL;
	Tools::debug("Logic: build_reply_msg:");
	if (mensaje_old != NULL)
	{
		mensaje = new Mensaje();
		mensaje->set_code(CODIGO_REPLY);
		mensaje->set_vendedor(nombre_nodo);
		mensaje->set_product_name(mensaje_old->get_product_name());		
		mensaje->set_cantidad(mensaje_old->get_cantidad());
		mensaje->set_hopcount(-1);
		mensaje->set_nodos(mensaje_old->get_nodos()); // Le paso el camino por donde debe vovler
	}
	else
	{
		Tools::error ("Logic: build_reply_msg: Mensaje recibido NULO");
	}
	return mensaje;
}

Mensaje *Logic::build_buy_msg(std::string vendedor, std::string product_name, int cantidad)
{
	Tools::debug("Logic: build_buy_msg:");
	Mensaje *mensaje = new Mensaje();
	mensaje->set_code(CODIGO_BUY);
	mensaje->set_vendedor(vendedor);
	mensaje->set_product_name(product_name);
	mensaje->set_cantidad(cantidad);
	mensaje->set_hopcount(-1);
	return mensaje;	
}


/**
 Locked Add Nodo
*/
void Logic::add_nodo(Mensaje *mensaje, const char*buffer)
{
	Tools::debug("Logic: add_nodo:");
	Nodo* nodo = new Nodo;
	nodo->name = Tools::duplicate(buffer);
	//clients[nodo->name] = nodo;
	mensaje->get_nodos().push_back(nodo);
}

void Logic::less_nodo(Mensaje *mensaje)
{
//http://www.cplusplus.com/reference/stl/list/
	Tools::debug("Logic: less_nodo:");
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
}

void Logic::less_hopcount(Mensaje *mensaje)
{
	Tools::debug("Logic: less_hopcount:");
	mensaje->set_hopcount(mensaje->get_hopcount()-1);
}

