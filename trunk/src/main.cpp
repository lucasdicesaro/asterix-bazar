
#include "comprador.h"
#include "vendedor.h"
#include "router.h"
#include "common/tools.h"
#include <iostream>
#include <pthread.h>
#include <signal.h>

void sig_handler(int id);
void* proc_comprador(void* param);
void* proc_vendedor(void* param);
void* proc_router(void* param);
//using namespace std;

char *nombre_nodo;
int listener_port;

int main()
{				
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);
	
	Tools tools;
	tools.Config_Parser("lista_participante.conf");
//	Tools::debug("ElTano");
//	Tools::debug(tools.get_IP_De_Participante("ElTano"));
	nombre_nodo = new char[NOMBRE_NODO_SIZE];
	memset(nombre_nodo, 0, NOMBRE_NODO_SIZE);
	nombre_nodo = tools.get_nombre_nodo();
	//Tools::debug("nombre_nodo");
	//Tools::debug(nombre_nodo);
	listener_port = tools.get_listener_port();
	//Tools::debug("listener_port");
	//Tools::debug(listener_port);
	
	
	Router* router = Router::instance();
	
	//Comprador* comprador = Comprador::instance();
	//Vendedor* vendedor = Vendedor::instance();
	
	pthread_t thr_comprador, thr_vendedor, thr_router;
	pthread_create(&thr_router, NULL, proc_router, router);

	/*
	int rubro = TURNO_COMPRADOR;
	if (rubro == TURNO_COMPRADOR) 
	{
		pthread_create(&thr_comprador, NULL, proc_comprador, comprador);
		pthread_join(thr_comprador, NULL);
	}
	else if (rubro == TURNO_VENDEDOR) 
	{
		pthread_create(&thr_vendedor, NULL, proc_vendedor, vendedor);
		pthread_join(thr_vendedor, NULL);
	}
	*/
	
	pthread_join(thr_router, NULL);
	Tools::debug("main. Finished");
}

void sig_handler(int id)
{
	if (id == SIGINT) 
	{
		Tools::debug("Se recibio la señal SIGINT");
	}
	
	Tools::debug("Termination Signal arrived");
	Event ev;
	ev.id = QUIT;
	//Comprador::instance()->post_event(ev, true);
	//Vendedor::instance()->post_event(ev, true);
	Router::instance()->post_event(ev, true);
}

void* proc_comprador(void* param)
{
	Tools::debug("proc_comprador: enter.");

	try
	{	
		Comprador* comprador = (Comprador*) param;
		comprador->run();
	} catch (...)
	{
		printf("Exception on comprador\n");
		exit(2);
	}

	Tools::debug("proc_comprador: leave.");
	return NULL;
}


void* proc_vendedor(void* param)
{
	Tools::debug("proc_vendedor: enter.");

	try
	{	
		Vendedor* vendedor = (Vendedor*) param;
		vendedor->run();
	} catch (...)
	{
		printf("Exception on vendedor\n");
		exit(2);
	}

	Tools::debug("proc_vendedor: leave.");
	return NULL;
}


void* proc_router(void* param)
{
	Tools::debug("proc_router: enter.");

	try
	{	
		Router* router = (Router*) param;
		router->run();
	} catch (...)
	{
		printf("Exception on router\n");
		exit(2);
	}

	Tools::debug("proc_router: leave.");
	return NULL;
}










