#ifndef _ROUTER_H_
#define _ROUTER_H_

#include "common/runner.h"
#include "common/types.h"

class Router : public Runner
{
	public:
		/**
		Singleton
		*/
		static Router* instance();
		/**
		* Implementacion requerida por Runner
		*/
		virtual void on_event(const Event& ev);

		void start_connections();		
		void close_TCP_connections();
		void send_broadcast_message(const void* msg);
		void send_to_socket_message(const void* msg);
		
	protected:	
		std::string get_connect_msg();
		int try_connection(const char *nombre_nodo);
	
	protected:
		Router();
		~Router();
		static Router* single_instance;				
		char *vecino1;
		char *vecino2;
		int sock_vecino1;
		int sock_vecino2;
		int intentos_reconexion;
		int delay_reconexion;
};

#endif //_ROUTER_H_
