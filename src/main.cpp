
#include "comprador.h"
#include "vendedor.h"
#include "common/tools.h"
#include <iostream>
#include <pthread.h>
#include <signal.h>

void sig_handler(int id);
void* proc_comprador(void* param);
void* proc_vendedor(void* param);
//using namespace std;

int main()
{				
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);
	
//	Tools tools;
//	tools.Config_Parser("lista_participante.conf");
//	Tools::debug("ElTano");
//	Tools::debug(tools.get_IP_De_Participante("ElTano"));
	
	Comprador* comprador = Comprador::instance();
	Vendedor* vendedor = Vendedor::instance();
	
	pthread_t thr_comprador, thr_vendedor;
	pthread_create(&thr_comprador, NULL, proc_comprador, comprador);
	pthread_create(&thr_vendedor, NULL, proc_vendedor, vendedor);
	
	// Wait threads:
	pthread_join(thr_comprador, NULL);
	pthread_join(thr_vendedor, NULL);
	
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
	Comprador::instance()->post_event(ev, true);
	Vendedor::instance()->post_event(ev, true);
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







