#ifndef _LISTENER_H_
#define _LISTENER_H_

#include "common/runner.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <map>

class Listener : public Runner
{
	public:
		typedef std::map<int, std::string> IpMapping;
		typedef std::map<int, std::string> NodoMapping;
		typedef std::map<int, std::string>::iterator NodoMappingIterator;
		/**
		Singleton
		*/
		static Listener* instance();
		/**
		* Implementacion requerida por Runner
		*/
		virtual void on_event(const Event& ev);

		void close_TCP_connections();
		
		int get_socket_from_client(std::string nombre_nodo);

	protected:
	
		/**
		* Administrador de sockets TCP:
		* Se espera por conexiones, se abren y cierran los sockets TCP con los clientes dependiendo del evento ocurrido
		*/	
		void client_connections_admin();
		void decode_mesage(char * buffer);
		
	
	protected:
		Listener();
		~Listener();
		static Listener* single_instance;				
		
	    int servSock;					// listening socket descriptor for server  
		struct sockaddr_in echoServAddr; // Local address
	    int port;
		IpMapping socket_ip_map;
        NodoMapping socket_nodo_map;       
};

#endif //_LISTENER_H_
