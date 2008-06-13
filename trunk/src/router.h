#ifndef _ROUTER_H_
#define _ROUTER_H_

#include "common/runner.h"
#include "common/types.h"
//#include <list>
#include <map>

class Router : public Runner
{
	public:
		typedef std::map<std::string, int> NodoMapping;
		typedef std::map<std::string, int>::iterator NodoMappingIterator;				
		
		//typedef std::list<RouterDS*> NodoSockets;
		//typedef std::list<RouterDS*>::iterator NodoSocketsIterator;
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
		
	protected:	
	
		void send_first_broadcast_message(const void* msg);
		void send_broadcast_message(const void* msg);
		
		void send_to_next_node_message(const void* msg);
		void send_to_socket_message(int id_socket, std::string mensaje_xml);				
			
		char *get_handshake_msg();
		void decode_rta_handshake_msg(const char *msg);
				
		void start_p2p_connect(const void* msg);
		void close_p2p_connect(const void* msg);				
		void send_to_p2p_socket_message(const void* msg);
		
		int try_connection(const char *nombre_nodo, bool socket_p2p = false);
		
		std::string find_in_server_sockets_nodo_nombre(int id_socket);
		std::string find_in_client_sockets_nodo_nombre(int id_socket);			
		
		void add_nodo_socket_into_server_map(std::string nombre_nodo, int id_socket);		
		void add_nodo_socket_into_client_map(std::string nombre_nodo, int id_socket);
		
		void rm_nodo_socket_from_server_map(std::string nombre_nodo);		
		void rm_nodo_socket_from_client_map(std::string nombre_nodo);
		
	protected:
		Router();
		~Router();
		static Router* single_instance;				
		char *vecino1;
		char *vecino2;
		char *p2p;		
		
		int sock_vecino1;
		int sock_vecino2;
		int sock_p2p;
		
		int intentos_reconexion;
		int delay_reconexion;
		NodoMapping nodo_socket_cliente_map;
		NodoMapping nodo_socket_servidor_map;
			
		bool show_menu;		
};

#endif //_ROUTER_H_
