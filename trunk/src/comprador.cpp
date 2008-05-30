
#include "comprador.h" 
#include "vendedor.h"
#include "common/tools.h"
#include <iostream>
#include <unistd.h> //sleep


Comprador* Comprador::single_instance = NULL;

/**
* Constructors
*/
Comprador::Comprador()
{
}

Comprador::~Comprador()
{
}

Comprador* Comprador::instance()
{
	if (!single_instance)
		single_instance = new Comprador;
	return single_instance;
}

void Comprador::on_event(const Event& ev)
{
	switch (ev.id)
	{
		case INIT:
			Tools::debug("Comprador: on_event: INIT:");
			procesar_init(ev.tag);
			break;
		
		// El cliente no recibe nunca LOOKUP
		//case LOOKUP:
		//	Tools::debug("Comprador: on_event: LOOKUP:");
		//	// Inicia la rutina de pedido de compra
		//	break;
			
		case REPLAY:
			Tools::debug("Comprador: on_event: REPLAY:");
			procesar_replay(ev.tag);
			break;
			
		// El cliente no recibe nunca BUY
		//case BUY:
		//	Tools::debug("Comprador: on_event: BUY:");
			// Inicia la rutina de compra			
		//	break;	
			
		case PRE_QUIT:
			Tools::debug("Comprador: on_event: QUIT:");
			procesar_pre_quit(ev.tag);
			break;
			
		default:
			Tools::debug("Comprador: on event: *UNKNOWN*.");
			break;
	}
}


void Comprador::procesar_init(const void *event_tag) 
{
	// Solo para prueba, despues quitar, porque el Comprador no va a estar en el mismo proceso que en Vendedor
	const char* producto = "Sal";
	Event ev;
	ev.id  = LOOKUP;
	ev.tag = producto;	
	Vendedor::instance()->post_event(ev, true);
}




void Comprador::procesar_replay(const void *event_tag) 
{
	// Procesa la respuesta del vendedor							
	// connect() al vendedor
	const char *vendedor = (char *)event_tag;
	Tools::debug (vendedor);
	
	Event ev;
	ev.id  = BUY;
	ev.tag = vendedor;	
	Vendedor::instance()->post_event(ev, true);
}


void Comprador::procesar_pre_quit(const void *event_tag) 
{
	// Inicia la rutina de salida	
	Event ev;
	ev.id  = QUIT;
	this->post_event(ev, true);
	//raise(SIGTERM);
}
