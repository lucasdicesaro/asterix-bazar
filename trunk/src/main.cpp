
#include "router.h"
#include "listener.h"
#include "logic.h"
#include "keyboard.h"
#include "stock.h"
#include "common/tools.h"
#include "common/logger.h"
#include "common/file_config_reader.h"
#include "asterix_file_config_parser.h"
#include <iostream>
#include <pthread.h>
#include <signal.h>

void sig_handler(int id);
void* proc_router(void* param);
void* proc_listener(void* param);
void* proc_logic(void* param);
void* proc_keyboard (void* param);

//using namespace std;

char *nombre_nodo;
int listener_port;
bool en_operacion = false;

int main(int argc, const char *argv[])
{				
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);
	
	char logBuffer[BUFFER_SIZE];

	//==========Levantando configuracion
	AsterixFileConfigParser *asterixFileConfigParser = new AsterixFileConfigParser();
	FileConfigReader::parseConfigFile(asterixFileConfigParser);

	Tools::instance()->set_lista_config(asterixFileConfigParser->get_lista_config());
	Tools::instance()->set_reconnect_params(asterixFileConfigParser->get_reconnect_params());
	Tools::instance()->set_current_stock(asterixFileConfigParser->get_current_stock());

	//==========Levantando configuracion de log
	Logger::instance()->parseConfigFile();
	/*
	Logger::debug("Main: probando debug");
	Logger::info("Main: probando info");
	Logger::warn("Main: probando warn");
	Logger::error("Main: probando error");
	*/

	// inicializa productos a comprar y vender segun parametros
	//if (argc > 2) 
	//{
	//	Stock* stock = Stock::instance();
	//	stock->set_compro(argv[1]);
	//	stock->set_vendo(argv[2]);
	//}
	//else 
	//{
	//	sprintf(logBuffer, "Parametros invalidos. Usage: \n%s <producto_compra> <producto_venta>", argv[0]);
	//	Logger::info(logBuffer);	
	//	exit(1);
	//}	

	nombre_nodo = new char[NOMBRE_NODO_SIZE];
	nombre_nodo = asterixFileConfigParser->get_nombre_nodo();
	std::string nombre_nodo_str = nombre_nodo;
	Logger::info("Main: nombre_nodo [" + nombre_nodo_str + "]");	
	
	listener_port = asterixFileConfigParser->get_listener_port();
	Logger::info_label_value("Main: listener_port", listener_port);
	
	//==========Iniciando hilos
	
	Router* router = Router::instance();
	Listener* listener = Listener::instance();		
	Logic* logic = Logic::instance();
	Keyboard * keyboard = Keyboard ::instance();        
	
	pthread_t thr_logic, thr_router, thr_listener, thr_keyboard;
	pthread_create(&thr_router, NULL, proc_router, router);
	pthread_create(&thr_listener, NULL, proc_listener, listener);
	pthread_create(&thr_logic, NULL, proc_logic, logic);
	pthread_create(&thr_keyboard, NULL, proc_keyboard , keyboard);

	pthread_join(thr_keyboard, NULL);	
	pthread_join(thr_router, NULL);
	pthread_join(thr_listener, NULL);
	pthread_join(thr_logic, NULL);

	Logger::debug("Main: Finished");
}

void sig_handler(int id)
{
	Logger::debug("Main: sig_handler: Termination Signal arrived");
	if (id == SIGINT) 
	{
		Logger::debug("Main: sig_handler: Se recibio la señal SIGINT");
	} 
	else if (id == SIGTERM) 
	{
		Logger::debug("Main: sig_handler: Se recibio la señal SIGTERM");
	}
	
	Listener::instance()->close_TCP_connections();
	Router::instance()->close_TCP_connections();
	
	Event ev;
	ev.id = QUIT;
	Router::instance()->post_event(ev, true);
	Listener::instance()->post_event(ev, true);
	Logic::instance()->post_event(ev, true);
	Keyboard::instance()->post_event(ev, true);	
	exit(15);	
}

void* proc_logic(void* param)
{
	Logger::debug("Main: proc_logic: enter.");

	try
	{	
		Logic* logic = (Logic*) param;
		logic->run(); // Cada n se pide una compra
	} catch (...)
	{
		printf("Main: Exception on logic\n");
		exit(2);
	}

	Logger::debug("Main: proc_logic: leave.");
	return NULL;
}


void* proc_router(void* param)
{
	Logger::debug("Main: proc_router: enter.");

	try
	{	
		Router* router = (Router*) param;
		router->run();
	} catch (...)
	{
		printf("Main: Exception on router\n");
		exit(2);
	}

	Logger::debug("Main: proc_router: leave.");
	return NULL;
}


void* proc_listener(void* param)
{
	Logger::debug("Main: proc_listener: enter.");

	try
	{	
		Listener* listener = (Listener*) param;
		listener->run();
	} catch (...)
	{
		printf("Main: Exception on listener\n");
		exit(2);
	}

	Logger::debug("Main: proc_listener: leave.");
	return NULL;
}

void* proc_keyboard(void* param)
{
	Logger::debug("Main: proc_keyboard: enter.");

	try
	{	
		Keyboard* keyboard = (Keyboard*) param;
		keyboard->run();
	} catch (...)
	{
		printf("Main: Exception on keyboard\n");
		exit(2);
	}

	Logger::debug("Main: proc_keyboard: leave.");
	return NULL;
}




