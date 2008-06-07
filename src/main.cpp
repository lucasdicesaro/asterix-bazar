
#include "router.h"
#include "listener.h"
#include "logic.h"
#include "common/tools.h"
#include <iostream>
#include <pthread.h>
#include <signal.h>

void sig_handler(int id);
void* proc_router(void* param);
void* proc_listener(void* param);
void* proc_logic(void* param);

//using namespace std;

char *nombre_nodo;
int listener_port;

int main()
{				
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);
	
	Tools* tools = Tools::instance();
	tools->Config_Parser("lista_participante.conf");
	
	nombre_nodo = new char[NOMBRE_NODO_SIZE];
	nombre_nodo = tools->get_nombre_nodo();
	std::string nombre_nodo_str = nombre_nodo;
	Tools::info("Main: nombre_nodo [" + nombre_nodo_str + "]");	
	
	listener_port = tools->get_listener_port();
	Tools::info_label_value("Main: listener_port", listener_port);
	
	
	Router* router = Router::instance();
	Listener* listener = Listener::instance();		
	Logic* logic = Logic::instance();
	
	pthread_t thr_logic, thr_router, thr_listener;
	pthread_create(&thr_router, NULL, proc_router, router);
	pthread_create(&thr_listener, NULL, proc_listener, listener);
	pthread_create(&thr_logic, NULL, proc_logic, logic);

	
	pthread_join(thr_router, NULL);
	pthread_join(thr_listener, NULL);
	pthread_join(thr_logic, NULL);	
	Tools::debug("Main: Finished");
}

void sig_handler(int id)
{
	Tools::debug("Main: Termination Signal arrived");
	if (id == SIGINT) 
	{
		Tools::debug("Main: Se recibio la señal SIGINT");
	} 
	else if (id == SIGTERM) 
	{
		Tools::debug("Main: Se recibio la señal SIGTERM");
	}
	
	Listener::instance()->close_TCP_connections();
	Router::instance()->close_TCP_connections();
	
	Event ev;
	ev.id = QUIT;
	Router::instance()->post_event(ev, true);
	Listener::instance()->post_event(ev, true);	
	exit(15);	
	
}

void* proc_logic(void* param)
{
	Tools::debug("Main: proc_logic: enter.");

	try
	{	
		Logic* logic = (Logic*) param;
		logic->run(); // Cada n se pide una compra
	} catch (...)
	{
		printf("Main: Exception on logic\n");
		exit(2);
	}

	Tools::debug("Main: proc_logic: leave.");
	return NULL;
}


void* proc_router(void* param)
{
	Tools::debug("Main: proc_router: enter.");

	try
	{	
		Router* router = (Router*) param;
		router->run();
	} catch (...)
	{
		printf("Main: Exception on router\n");
		exit(2);
	}

	Tools::debug("Main: proc_router: leave.");
	return NULL;
}


void* proc_listener(void* param)
{
	Tools::debug("Main: proc_listener: enter.");

	try
	{	
		Listener* listener = (Listener*) param;
		listener->run();
	} catch (...)
	{
		printf("Main: Exception on listener\n");
		exit(2);
	}

	Tools::debug("Main: proc_listener: leave.");
	return NULL;
}









