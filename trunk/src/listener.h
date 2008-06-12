#ifndef _LISTENER_H_
#define _LISTENER_H_

#include "common/runner.h"
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <map>

class Listener : public Runner
{
	public:
		typedef std::map<int, std::string> IpMapping;
		/**
		Singleton
		*/
		static Listener* instance();
		/**
		* Implementacion requerida por Runner
		*/
		virtual void on_event(const Event& ev);

		void close_TCP_connections();

	protected:
	
		/**
		* Administrador de sockets TCP:
		* Se espera por conexiones, se abren y cierran los sockets TCP con los clientes dependiendo del evento ocurrido
		*/	
		void client_connections_admin();
		void prepare_connections();
		
		void add_socket(int id_socket);
		void rm_socket(int id_socket);
				
		void decode_handshake_msg(const char *msg);
		char *get_rta_handshake_msg();		
		

	protected:
		Listener();
		~Listener();
		static Listener* single_instance;				
		
	    int servSock;					// listening socket descriptor for server  
		fd_set master;   // master file descriptor list
		fd_set read_fds; // temp file descriptor list for select()
		int fdmax;        // maximum file descriptor number	    		
		struct sockaddr_in echoServAddr; // Local address		
	    struct sockaddr_in remoteaddr; // client address
		int newfd;        // newly accept()ed socket descriptor
		int nbytes;			//Size of received message
		socklen_t addrlen;		
		int cant_clientes;
		
		char logBuffer[BUFFER_SIZE];        // Buffer for log
		char echoBuffer[BUFFER_SIZE];       // Buffer for echo string 		
		
	    int port;
		IpMapping socket_ip_map;		
		struct timeval timeout;		
};

#endif //_LISTENER_H_
