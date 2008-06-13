

#include "common/tools.h"
#include "common/types.h"
#include "logic.h"
#include "router.h"
#include "stock.h"
#include "common/tools.h"
#include <assert.h>

extern char *nombre_nodo;

Logic* Logic::single_instance = NULL;

/**
* Constructors
*/
Logic::Logic()
{
	ReconnectParamsDS *reconnectParams = Tools::instance()->get_reconnect_params();
	set_hopcount(reconnectParams->hopcount);
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
	int cantidad;	
	switch (ev.id)
	{
		int cantidad;
		case INIT:
			Tools::debug("Logic: on_event: INIT:");
			break;		
			
		case DO_LOOKUP:
			Tools::debug("Logic: on_event: DO_LOOKUP:");
			// TODO Recibir la cantidad por teclado
			cantidad = 2;
			on_send_look_up(Stock::instance()->get_compro(), cantidad);			
			break;
			
		case DO_LOOKUP_FORWARD:
			Tools::debug("Logic: on_event: DO_LOOKUP_FORWARD:");			
			on_send_look_up_forward(ev.tag);
			break;								
			
		case DO_REPLY:
			Tools::debug("Logic: on_event: DO_REPLY:"); 
			on_send_replay(ev.tag);
			break;
						
		case DO_REPLY_FORWARD:
			Tools::debug("Logic: on_event: DO_REPLY_FORWARD:");
			on_send_replay_forward(ev.tag);			
			break;			
			
		case DO_BUY:
			Tools::debug("Logic: on_event: DO_BUY:");
			on_send_buy(ev.tag);			
			break;		
									
		case DO_BUY_REPLY_OK:
			Tools::debug("Logic: on_event: DO_BUY_REPLY_OK:"); 
			on_send_buy_reply_ok(ev.tag);
			break;			
			
		case DO_BUY_REPLY_ERR:
			Tools::debug("Logic: on_event: DO_BUY_REPLY_ERR:"); 
			on_send_buy_reply_err(ev.tag);
			break;						
			
		case CLIENT_MSG:
			Tools::debug("Logic: on_event: CLIENT_MSG:");
			on_client_msg(ev.tag);
			break;			

		case SET_STOCK_SAL:
			Tools::debug("Logic: on_event: SET_STOCK_SAL");
			buffer = (char*) ev.tag;		
			cantidad = atoi(buffer);		
			set_stock_product(PRODUCTO_SAL, cantidad);
			break;			
		  
		default:
			Tools::debug("Logic: on event: *UNKNOWN*.");
			break;
	}
}



void Logic::set_stock_product(std::string product_name, int cantidad)
{
	char logBuffer[BUFFER_SIZE];
	sprintf(logBuffer, "Se setea %d unidades de %s en el stock", cantidad, product_name.c_str());
	Tools::info(logBuffer);
	
	Stock::instance()->set_stock(product_name, cantidad);
	Stock::instance()->to_string();
}


void Logic::on_send_look_up(std::string product_name, int cantidad)
{
	char logBuffer[BUFFER_SIZE];
	
	// Obtiene producto a comprar de clase STOCK
	Mensaje *mensaje = build_look_up_msg(product_name, cantidad);
	add_nodo(mensaje, nombre_nodo);
		
	Event evRta;
	evRta.id = RT_FIRST_BROADCAST;
	evRta.tag = Tools::duplicate(mensaje->to_string());
	Router::instance()->post_event(evRta, true);	
}

void Logic::on_send_look_up_forward(const void* msg)
{
	Mensaje *mensaje = (Mensaje *)msg;
	char logBuffer[BUFFER_SIZE];
	
	if (mensaje != NULL)
	{						
		if (mensaje->get_hopcount() > 0)				
		{			
			less_hopcount(mensaje);
			add_nodo(mensaje, nombre_nodo);					
			Tools::debug("Logic: on_send_look_up_forward: Reenviando el LOOKUP");					
			
			Event evRta;
			evRta.id = RT_BROADCAST;
			evRta.tag = mensaje;
			Router::instance()->post_event(evRta, true);					
		}
		else 
		{
			Tools::debug("Logic: on_send_look_up_forward: Se descarto el mensaje, por agotarse su hopcount");
			sprintf(logBuffer, "Se descarto el mensaje originado por [%s], por agotarse su hopcount. El mensaje buscaba %d unidades de [%s]", 
				mensaje->get_creator_node_name().c_str(), mensaje->get_cantidad(), mensaje->get_product_name().c_str());			
			Tools::info(logBuffer);
		}	
	}
	else
	{
		Tools::error("Logic: on_send_look_up_forward: mensaje es NULL en DO_LOOKUP_FORWARD. Se descarta");
	}
}

void Logic::on_send_replay(const void* msg)
{
	Mensaje *mensaje_old = (Mensaje *)msg;
	Mensaje *mensaje = build_reply_msg(mensaje_old);
	if (mensaje != NULL)
	{
		Tools::debug("Logic: on_send_replay: Enviando el REPLY");
		
		Event evRta;
		evRta.id = RT_SEND_TO_SOCKET;
		evRta.tag = mensaje;
		Router::instance()->post_event(evRta, true);					
	}
	else
	{
		Tools::error("Logic: on_send_replay: mensaje es NULL en DO_REPLY_FORWARD. Operacion descatada");
	}		
}

void Logic::on_send_replay_forward(const void* msg)
{
	Mensaje *mensaje = (Mensaje *)msg;
	if (mensaje != NULL)
	{
		Tools::debug("Logic: on_send_replay_forward: Reenviando el REPLY");
		less_nodo(mensaje);		
		
		Event evRta;
		evRta.id = RT_SEND_TO_SOCKET;
		evRta.tag = mensaje;
		Router::instance()->post_event(evRta, true);					
	}
	else
	{
		Tools::error("Logic: on_send_replay_forward: mensaje es NULL en DO_REPLY_FORWARD");
	}
}

void Logic::on_send_buy(const void* msg)
{
	Tools::debug("Logic::on_send_buy:");
	char logBuffer[BUFFER_SIZE];

	Mensaje *mensaje_old = (Mensaje *)msg;		
	Mensaje *mensaje = build_buy_msg(mensaje_old);
	
	sprintf(logBuffer, "Voy a iniciar la negociacion con [%s]", mensaje->get_vendedor().c_str());
	Tools::info(logBuffer);	
			
	Event evRta;
	evRta.id = RT_START_P2P_CONNECT;
	evRta.tag = mensaje;
	Router::instance()->post_event(evRta, true);
}


void Logic::on_send_buy_reply_ok(const void* msg)
{
	Tools::debug("Logic::on_send_buy_reply_ok:");
	char logBuffer[BUFFER_SIZE];
	Mensaje *mensaje_old = (Mensaje *)msg;		
	Mensaje *mensaje = build_buy_reply_ok_msg(mensaje_old);
	
	sprintf(logBuffer, "Operacion existosa. Le aviso al comprador que incremente lo suyo");
	Tools::info(logBuffer);					
	
	Event evRta;
	evRta.id = RT_SEND_TO_P2P_SOCKET;
	evRta.tag = mensaje;
	Router::instance()->post_event(evRta, true);
}

void Logic::on_send_buy_reply_err(const void* msg)
{
	Tools::debug("Logic::on_send_buy_reply_err:");
	char logBuffer[BUFFER_SIZE];
	Mensaje *mensaje_old = (Mensaje *)msg;		
	Mensaje *mensaje = build_buy_reply_err_msg(mensaje_old);
	
	sprintf(logBuffer, "Operacion fallida. No tuve stock suficiente en el momento en que se realizo la negociacion. Le aviso al comprador que no incremente lo suyo", nombre_nodo);
	Tools::info(logBuffer);
				
	Event evRta;
	evRta.id = RT_SEND_TO_P2P_SOCKET;
	evRta.tag = mensaje;
	Router::instance()->post_event(evRta, true);
}





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
	
	if (mensaje->get_code().compare(CODIGO_LOOKUP) == 0) 
	{
		on_receive_look_up(mensaje);
	}
	else if (mensaje->get_code().compare(CODIGO_REPLY) == 0)
	{
		on_receive_replay(mensaje);
	}
	else if (mensaje->get_code().compare(CODIGO_BUY) == 0)
	{
		on_receive_buy(mensaje);
	}
	else if (mensaje->get_code().compare(CODIGO_BUY_OK) == 0)
	{
		on_receive_buy_reply_ok(mensaje);
	}	
	else if (mensaje->get_code().compare(CODIGO_BUY_ERR) == 0)
	{
		on_receive_buy_reply_err(mensaje);
	}	
}

void Logic::on_receive_look_up(Mensaje *mensaje)
{
	Stock* stock = Stock::instance();
		
	char logBuffer[BUFFER_SIZE];
	memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Logic: on_receive_look_up: Recibido del Lookup: [%d] unidades de [%s]", 
			mensaje->get_cantidad(), mensaje->get_product_name().c_str());
	Tools::debug(logBuffer);
	
	// Checkea stock para determinar si vendo lo solicitado y tengo stock	
	Event ev;
	ev.tag = mensaje;	
	std::string producto_solicitado = mensaje->get_product_name();
	int cantidad_solicitada = mensaje->get_cantidad();
	char* vendo = stock->get_vendo();
	
	memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Logic: on_receive_look_up: Producto solicitado [%s] y vendo [%s]", 
			mensaje->get_product_name().c_str(), stock->get_vendo());
	Tools::debug(logBuffer);
	
	if (producto_solicitado.compare(vendo) == 0)
	{
		sprintf(logBuffer, "Yo [%s], vendo lo que me estan pidiendo [%s]", nombre_nodo, mensaje->get_product_name().c_str());
		Tools::info(logBuffer);
		if (stock->get_stock(producto_solicitado) >= cantidad_solicitada)
		{
			sprintf(logBuffer, "y tengo la cantidad (%d) que me piden %d. Respondo el mensaje", stock->get_stock(producto_solicitado), mensaje->get_cantidad());			
			Tools::info(logBuffer);
			
			ev.id = DO_REPLY;
		}
		else
		{
			sprintf(logBuffer, ", pero NO tengo la cantidad (%d) que me piden %d. Reenvio el mensaje", stock->get_stock(producto_solicitado), mensaje->get_cantidad());			
			Tools::info(logBuffer);
			
			ev.id = DO_LOOKUP_FORWARD;
		}
	}
	else 
	{
		sprintf(logBuffer, "Yo [%s], NO vendo lo que me estan pidiendo [%s]. Reenvio el mensaje", nombre_nodo, mensaje->get_product_name().c_str());
		Tools::info(logBuffer);
		
		ev.id = DO_LOOKUP_FORWARD;
	}
	
	this->post_event(ev, true);	
}

void Logic::on_receive_replay(Mensaje *mensaje)
{
	Tools::debug("Logic: on_receive_replay:");
	char logBuffer[BUFFER_SIZE];
	

	// Si queda un nodo solo en el mensaje, 
	// el nombre de ese nodo, tiene que coincidir con el nombre de nodo que lo inicio.
	Event ev;		
	std::string last_nodo_name;
	if (mensaje->count() <= 1)
	{
		last_nodo_name = mensaje->get_next_node_name();
		memset(logBuffer, 0 , sizeof(logBuffer));
		sprintf(logBuffer, "Logic: on_receive_replay: Queda solo un nodo en el mensaje [%s]. Cant [%d]", last_nodo_name.c_str(), mensaje->count());
		Tools::debug(logBuffer);			
		
		if (last_nodo_name.compare(nombre_nodo) == 0)
		{
			Tools::debug("Logic: on_receive_replay: El ultimo nodo contiene mi nombre");
			memset(logBuffer, 0 , sizeof(logBuffer));
			sprintf(logBuffer, "El vendedor [%s] vende lo que necesito: [%d] unidades de [%s]", mensaje->get_vendedor().c_str(), mensaje->get_cantidad(), mensaje->get_product_name().c_str());
			Tools::info(logBuffer);
			
			ev.id = DO_BUY;
			ev.tag = mensaje;
			this->post_event(ev, true);				
		}
		else
		{
			Tools::warn("Logic: on_receive_replay: El ultimo nodo No contiene mi nombre: No deberia llegar aqui. Descarto el mensaje");			
		}
	}
	else
	{
		Tools::debug("Logic: on_codigo_replay: Sigue quedando mas de un nodo en el mensaje. Se reenvia");
		
		memset(logBuffer, 0 , sizeof(logBuffer));
		sprintf(logBuffer, "Se traspasa la respuesta de compra hasta llegar a [%s] (nodo que origino el pedido)", mensaje->get_creator_node_name().c_str());
		Tools::info (logBuffer);
			
		ev.id = DO_REPLY_FORWARD;
		ev.tag = mensaje;
		this->post_event(ev, true);
	}
	
}

void Logic::on_receive_buy(Mensaje *mensaje)
{
	Tools::debug("Logic: on_receive_buy:");
	char logBuffer[BUFFER_SIZE];
	
	sprintf(logBuffer, "El comprador inicio la negociacion conmigo");
	Tools::info(logBuffer);
		
	Event ev;
	if (Stock::instance()->get_stock(mensaje->get_product_name().c_str()) >= mensaje->get_cantidad())
	{		
		Stock::instance()->decrement_stock(mensaje->get_product_name().c_str(), mensaje->get_cantidad());
		sprintf(logBuffer, "Decremente de mi stock de [%s] en %d unidades. Operacion Exitosa. Le comunico al comprador", mensaje->get_product_name().c_str(), mensaje->get_cantidad());		
		Tools::info(logBuffer);		
		ev.id = DO_BUY_REPLY_OK;
	}		
	else
	{
		sprintf(logBuffer, "No tengo stock suficiente para vender [%s]. Tengo %d unidades y me piden %d unidades. Le respondo negativamente", mensaje->get_product_name().c_str(), Stock::instance()->get_stock(mensaje->get_product_name().c_str()), mensaje->get_cantidad());
		Tools::info(logBuffer);	
		ev.id = DO_BUY_REPLY_ERR;
	}
	Stock::instance()->to_string();	
	
	ev.tag = mensaje;
	this->post_event(ev, true);		
}


void Logic::on_receive_buy_reply_ok(Mensaje *mensaje)
{
	Tools::debug("Logic: on_receive_buy_reply_ok:");
	char logBuffer[BUFFER_SIZE];
	
	Stock::instance()->increment_stock(mensaje->get_product_name().c_str(), mensaje->get_cantidad());
	sprintf(logBuffer, "Incremente mi stock de [%s] en %d unidades. Operacion finalizada", mensaje->get_product_name().c_str(), mensaje->get_cantidad());		
	Tools::info(logBuffer);	

	Stock::instance()->to_string();
	
	Event evRta;
	evRta.id = RT_CLOSE_P2P_CONNECT; // En esta instancia soy el comprador. debo cerrar el socket p2p
	evRta.tag = mensaje;
	Router::instance()->post_event(evRta, true);	
}

void Logic::on_receive_buy_reply_err(Mensaje *mensaje)
{
	Tools::debug("Logic: on_receive_buy_reply_err:");
	char logBuffer[BUFFER_SIZE];
	sprintf(logBuffer, "El vendedor [%s] no pudo satisfacer mi pedido. No se modifica el stock. Operacion finalizada", mensaje->get_vendedor().c_str(), mensaje->get_product_name().c_str(), mensaje->get_cantidad());
	Tools::info(logBuffer);	
	
	Stock::instance()->to_string();
					
	Event evRta;
	evRta.id = RT_CLOSE_P2P_CONNECT; // En esta instancia soy el comprador. debo cerrar el socket p2p
	evRta.tag = mensaje;
	Router::instance()->post_event(evRta, true);	
}



Mensaje *Logic::build_look_up_msg(std::string product_name, int cantidad)
{
	Tools::debug("Logic: build_look_up_msg:");
	Mensaje *mensaje = new Mensaje();
	mensaje->set_code(CODIGO_LOOKUP);
	mensaje->set_product_name(product_name);
	mensaje->set_cantidad(cantidad);
	mensaje->set_hopcount(get_hopcount());
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
		mensaje->set_timestamp(mensaje->get_timestamp());		
		mensaje->set_nodos(mensaje_old->get_nodos()); // Le paso el camino por donde debe volver
	}
	else
	{
		Tools::error ("Logic: build_reply_msg: Mensaje recibido NULO");
	}
	return mensaje;
}

Mensaje *Logic::build_buy_msg(Mensaje *mensaje_old)
{
	Mensaje *mensaje = NULL;
	Tools::debug("Logic: build_buy_msg:");
	if (mensaje_old != NULL)
	{
		mensaje = new Mensaje();
		mensaje->set_code(CODIGO_BUY);
		mensaje->set_vendedor(mensaje_old->get_vendedor());
		mensaje->set_product_name(mensaje_old->get_product_name());		
		mensaje->set_cantidad(mensaje_old->get_cantidad());
		mensaje->set_hopcount(-1);
		mensaje->set_timestamp(mensaje->get_timestamp());		
	}
	else
	{
		Tools::error ("Logic: build_buy_msg: Mensaje recibido NULO");
	}
	return mensaje;		
}

Mensaje *Logic::build_buy_reply_ok_msg(Mensaje *mensaje_old)
{
	Mensaje *mensaje = NULL;
	Tools::debug("Logic: build_buy_ok_msg:");
	if (mensaje_old != NULL)
	{
		mensaje = new Mensaje();
		mensaje->set_code(CODIGO_BUY_OK);
		mensaje->set_vendedor(mensaje_old->get_vendedor());
		mensaje->set_product_name(mensaje_old->get_product_name());		
		mensaje->set_cantidad(mensaje_old->get_cantidad());
		mensaje->set_hopcount(-1);
		mensaje->set_timestamp(mensaje->get_timestamp());			
	}
	else
	{
		Tools::error ("Logic: build_buy_msg: Mensaje recibido NULO");
	}
	return mensaje;		
}


Mensaje *Logic::build_buy_reply_err_msg(Mensaje *mensaje_old)
{
	Mensaje *mensaje = NULL;
	Tools::debug("Logic: build_buy_ok_msg:");
	if (mensaje_old != NULL)
	{
		mensaje = new Mensaje();
		mensaje->set_code(CODIGO_BUY_ERR);
		mensaje->set_vendedor(mensaje_old->get_vendedor());
		mensaje->set_product_name(mensaje_old->get_product_name());		
		mensaje->set_cantidad(mensaje_old->get_cantidad());
		mensaje->set_hopcount(-1);
		mensaje->set_timestamp(mensaje->get_timestamp());			
	}
	else
	{
		Tools::error ("Logic: build_buy_msg: Mensaje recibido NULO");
	}
	return mensaje;		
}




void Logic::add_nodo(Mensaje *mensaje, const char*buffer)
{
	Tools::debug("Logic: add_nodo:");
	Nodo* nodo = new Nodo;
	nodo->name = Tools::duplicate(buffer);
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
	delete nodo;	
}

void Logic::less_hopcount(Mensaje *mensaje)
{
	Tools::debug("Logic: less_hopcount:");
	mensaje->set_hopcount(mensaje->get_hopcount()-1);
}

void Logic::set_hopcount(int hopcount)
{
	this->hopcount = hopcount;
}
int Logic::get_hopcount()
{
	return hopcount;
}


