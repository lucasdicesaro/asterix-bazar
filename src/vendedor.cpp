
#include "comprador.h" // Quitar esto despues de las pruebas
#include "vendedor.h"
#include "common/tools.h"
#include <iostream>

Vendedor* Vendedor::single_instance = NULL;

/**
* Constructors
*/
Vendedor::Vendedor()
{
}

Vendedor::~Vendedor()
{
}

Vendedor* Vendedor::instance()
{
	if (!single_instance)
		single_instance = new Vendedor;
	return single_instance;
}

void Vendedor::on_event(const Event& ev)
{
	switch (ev.id)
	{
		case INIT:
			Tools::debug("Vendedor: on_event: INIT:");
			break;
			
		case LOOKUP:
			Tools::debug("Vendedor: on_event: LOOKUP:");
			procesar_look_up(ev.tag);
			break;
			
// El vendedor nunca recibe REPLAY			
//		case REPLAY:
//			Tools::debug("Vendedor: on_event: REPLAY:");
//			// Inicia la rutina de respuesta al comprador
//			break;
						
		case BUY:
			Tools::debug("Vendedor: on_event: BUY:");
			procesar_buy(ev.tag);			
			break;	
			
		case PRE_QUIT:
			Tools::debug("Vendedor: on_event: QUIT:");
			procesar_pre_quit(ev.tag);
			break;	
			
		default:
			Tools::debug("Vendedor: on event: *UNKNOWN*.");
			break;
	}
}

void Vendedor::procesar_look_up(const void *event_tag) 
{
	// Procesa el pedido de compra del comprador
	const char *producto = (char *)event_tag;
	Tools::debug (producto);
	
	const char* vendedor = "LaPetisita";
	Event ev;
	ev.id  = REPLAY;
	ev.tag = vendedor;	
	Comprador::instance()->post_event(ev, true);
}




void Vendedor::procesar_buy(const void *event_tag) 
{
	// Procesa el pedido de compra.	
	// accept() . El vendedor acepta la conexion del cliente
	const char *vendedor = (char *)event_tag;
	Tools::debug (vendedor);
				
	Event ev;
	ev.id  = PRE_QUIT;
	Comprador::instance()->post_event(ev, true);
	this->post_event(ev, true);
}


void Vendedor::procesar_pre_quit(const void *event_tag) 
{
	// Inicia la rutina de salida
	
	Event ev;
	ev.id  = QUIT;
	this->post_event(ev, true);
}
