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

		void procesar_start_connection(const void *event_tag);
		void procesar_pre_quit(const void *event_tag);
		void close_TCP_connections();
		//void procesar_look_up(const void *event_tag);
		//void procesar_buy(const void *event_tag);
		void decode_mesage(char * buffer);
		void send_message(const void* msg);
	
	protected:
		Router();
		~Router();
		static Router* single_instance;				
		char *vecino1;
		char *vecino2;
		int sock_vecino1;
		int sock_vecino2;
		
		//int sock;
		//const char* serverIP;				// Server IP address (dotted quad)
		//unsigned short serverPort;	// Server port
		//std::string playerId;		
};

#endif //_ROUTER_H_
