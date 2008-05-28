#ifndef _ROUTER_H_
#define _ROUTER_H_

#include "common/runner.h"
#include "common/types.h"
#include <map>

class Router : public Runner
{
	public:
		typedef std::map<int, std::string> IpMapping;
		/**
		Singleton
		*/
		static Router* instance();
		/**
		* Implementacion requerida por Runner
		*/
		virtual void on_event(const Event& ev);
		
		void procesar_init(const void *event_tag);
		void procesar_start_connection(const void *event_tag);
		void procesar_wait_connections(const void *event_tag);
		void procesar_pre_quit(const void *event_tag);
		//void procesar_look_up(const void *event_tag);
		//void procesar_buy(const void *event_tag);
		void decode_mesage(char* buffer);
		std::string get_connect_msg();

	
	protected:
		Router();
		~Router();
		static Router* single_instance;				
		char *vecino1;
		char *vecino2;
		int sock_vecino1;
		int sock_vecino2;
		int servSock;
		IpMapping socket_ip_map;
};

#endif //_ROUTER_H_
