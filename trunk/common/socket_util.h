/*
 * Funciones para que un cliente pueda abrir sockets con un servidor.
 */
#ifndef _SOCKET_UTIL_H_
#define _SOCKET_UTIL_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>

class SocketUtil
{
	public:
		
		/**
		 * Abre un socket TCP con un servidor  
		 */
		int static cliente_abrir_conexion_tcp (const char *server_ip, int serverPort);
		
		char static *recibir_mensaje(int socket);

		/**
		 * Abre un socket UDP con un servidor  
		 */
		int static cliente_abrir_conexion_udp (struct sockaddr_in *localAddr);
		

		/**
		 * Abre un socket TCP para atender al servicio
		 * Devuelve el descriptor del socket que atiende a ese servicio o -1 si ha habido error.
		 */
		int static servidor_abrir_conexion_tcp(int listenerPort);


		/**
		 * Abre un socket UDP para atender al servicio.
		 * Devuelve el descriptor del socket que atiende a ese servicio o -1 si ha habido error.
		 */
		int static servidor_abrir_conexion_udp(struct sockaddr_in *localAddr);


		/**
		 * Acepta un cliente para un socket INET.
		 * Devuelve el descriptor de la conexión con el cliente o -1 si ha habido error.
		 */
		int static servidor_aceptar_conexion_cliente(int sock);

		void static servidor_nuevo_cliente(int sockServidor, int *clientes, int *nClientes);		
		
		int static obtener_maximo_fd(int *tabla, int n);

		void static limpiar_tabla_clientes(int *tabla, int *n);

	protected:

};
#endif //_SOCKET_UTIL_H_
