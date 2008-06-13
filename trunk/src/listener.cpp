

#include "common/tools.h"
#include "common/types.h"
#include "common/socket_util.h"
#include "listener.h"
#include "logic.h"
#include "router.h"
#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <string>

extern char *nombre_nodo;
extern int listener_port;
extern bool en_operacion;

Listener* Listener::single_instance = NULL;

/**
* Constructors
*/
Listener::Listener()
{
	cant_clientes = 0;
	sock_p2p = SOCK_ERRONEO;
}

Listener::~Listener()
{
}

Listener* Listener::instance()
{
	if (!single_instance)
		single_instance = new Listener;
	return single_instance;
}

void Listener::on_event(const Event& ev)
{
	int id_socket;
	std::string socket;
	switch (ev.id)
	{
		case INIT:
			Tools::debug("Listener: on_event: INIT:");
			prepare_connections();
			break;
			
		case LOOP:
			//Tools::debug("Listener: on_event: LOOP:");
			client_connections_admin();
			break;			
			
		case LS_ADD_SOCKET:
			Tools::debug("Listener: on_event: LS_ADD_SOCKET:");
			socket = (char*)ev.tag;			
			id_socket = atoi(socket.c_str());
			add_socket(id_socket);
			break;
			
		case LS_RM_SOCKET:
			Tools::debug("Listener: on_event: LS_RM_SOCKET:");
			socket = (char*)ev.tag;			
			id_socket = atoi(socket.c_str());
			rm_socket(id_socket);
			break;			
			
		case LS_ADD_SOCKET_P2P:
			Tools::debug("Listener: on_event: LS_ADD_SOCKET_P2P:");
			socket = (char*)ev.tag;			
			id_socket = atoi(socket.c_str());
			add_socket_p2p(id_socket);
			break;			
			
		case LS_RM_SOCKET_P2P:
			Tools::debug("Listener: on_event: LS_RM_SOCKET_P2P:");
			socket = (char*)ev.tag;			
			id_socket = atoi(socket.c_str());			
			rm_socket_p2p(id_socket);
			break;						
			
		default:
			Tools::debug("Listener: on event: *UNKNOWN*.");
			break;
	}
}

void Listener::prepare_connections()
{    
	Tools::debug("Listener: prepare_connections:");
    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);
	
	servSock = SocketUtil::servidor_abrir_conexion_tcp(listener_port);
	
    // add the servSock to the master set
    FD_SET(servSock, &master);

    // keep track of the biggest file descriptor
    fdmax = servSock; // so far, it's this one
	
	Event ev;
	ev.id = LOOP;
	this->post_event(ev, true);  	
}

void Listener::client_connections_admin()
{   
    timeout.tv_sec = DEFAULT_WAIT_SECONDS;// Timeout del select
    timeout.tv_usec = DEFAULT_WAIT_MILISECONDS;	
	
    read_fds = master; // copy it
	
	int retornoSelect;
	if ((retornoSelect = select(fdmax+1, &read_fds, NULL, NULL, &timeout)) == -1) 
    {
        Tools::error("Listener: client_connections_admin: select");
        return;
    }
	else if (retornoSelect == 0)
	{   
		//Tools::debug(nombre_nodo, "Listener: client_connections_admin: el select volvio por timeout");
	}
	else if (retornoSelect > 0) 
	{
		//Tools::debug(nombre_nodo, "Listener: client_connections_admin: el select volvio por escritura en los descriptores");


        // run through the existing connections looking for data to read
		int i;
        for(i = 0; i <= fdmax; i++) 
        {
	        if (FD_ISSET(i, &read_fds)) 
	        { // we got one!!
	        
                if (i == servSock) 
                {
                  // handle new connections
                  //if (Broadcaster::instance()->get_count() < MAX_CONNECTED)
				  if (cant_clientes < MAX_CONNECTED)
                  {
                    addrlen = sizeof(remoteaddr);
                    if ((newfd = accept(servSock, (struct sockaddr *)&remoteaddr, &addrlen)) == -1) 
                    { 
                        Tools::error("Listener: accept");
                    } 
                    else 
                    {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) 
                        {    // keep track of the maximum
                            fdmax = newfd;
                        }
                        
						char ip[32];
						memset(ip, 0 , sizeof(ip));
						strcpy(ip, inet_ntoa(remoteaddr.sin_addr));
												
						socket_ip_map[newfd] = ip;// Asignacion al mapa de ip's
						
						cant_clientes++;
						
						memset(logBuffer, 0 , sizeof(logBuffer));
						sprintf(logBuffer, "Listener: client_connections_admin: Nueva conexion de [%s] con el socket %d", inet_ntoa(remoteaddr.sin_addr), newfd);
						Tools::debug(logBuffer);
						
						char *nombre_nodo_vecino = new char[BUFFER_SIZE];
						// Recibo el nombre de nodo de mi vecino
						memset(nombre_nodo_vecino, 0, BUFFER_SIZE);
						nombre_nodo_vecino = SocketUtil::recibir_mensaje(newfd);						
						
						Event evRouter;
						char *buffer_nodo_socket_id = new char[BUFFER_SIZE];						
						memset(buffer_nodo_socket_id, 0, BUFFER_SIZE);						
						// determino si la nueva conexion es el socket p2p
						if (strcmp(nombre_nodo_vecino, P2P_CONNECT) == 0)
						{
							sprintf(logBuffer, "Entro en el estado 'en operacion'");
							Tools::info(logBuffer);							
							en_operacion = true;
							
							sprintf(logBuffer, "Listener: client_connections_admin: La nueva conexion es un socket p2p");
							Tools::debug(logBuffer);
							
							sock_p2p = newfd;
							
							memset(nombre_nodo_vecino, 0, BUFFER_SIZE);
							nombre_nodo_vecino = SocketUtil::recibir_mensaje(newfd);						
							decode_handshake_msg(nombre_nodo_vecino);
							
							sprintf(buffer_nodo_socket_id, "%d", newfd);								
							
							evRouter.id = RT_SET_SOCKET_P2P;							
							evRouter.tag = Tools::duplicate(buffer_nodo_socket_id);									
						}
						else
						{	
							// La nueva conexion, no es la conexion p2p
							decode_handshake_msg(nombre_nodo_vecino);
						
							sprintf(buffer_nodo_socket_id, "%s,%d", nombre_nodo_vecino, newfd);								
							
							evRouter.id = RT_ADD_NODO_SERVIDOR;							
							evRouter.tag = Tools::duplicate(buffer_nodo_socket_id);											
						}
						
						// Armo el mensaje con mi nombre de nodo y lo mando a mi vecino
						char *rta_hndsk = get_rta_handshake_msg();
						SocketUtil::enviar_mensaje(newfd, rta_hndsk);						
										
						Router::instance()->post_event(evRouter, true);
                     }
                  }
				  else 
				  {
					  Tools::error("Listener: client_connections_admin: Se llego al limite de conexiones aceptadas por el Listener");
					  Tools::debug_label_value ("Listener: client_connections_admin: Maximo de clientes conectados", MAX_CONNECTED);
				  }
                } 
                else 
                {                					
   					memset(echoBuffer, 0, BUFFER_SIZE);
   					
                    // handle data from a client
                    if ((nbytes = recv(i, echoBuffer, sizeof(echoBuffer), 0)) <= 0) 
                    {
                    
                        // got error or connection closed by client
                        if (nbytes == 0) 
                        {
							memset(logBuffer, 0 , sizeof(logBuffer));
    		                sprintf(logBuffer, "Listener: client_connections_admin: Connection closed (socket %d)", i);
							Tools::debug(logBuffer);
							
							if (!socket_ip_map[i].empty()) 
							{
								memset(logBuffer, 0 , sizeof(logBuffer));
						        sprintf(logBuffer, "Listener: client_connections_admin: Removiendo la IP [%s]", socket_ip_map[i].c_str());
								Tools::debug(logBuffer);
								
								memset(logBuffer, 0 , sizeof(logBuffer));
						        sprintf(logBuffer, "Removiendo la IP [%s]", socket_ip_map[i].c_str());
								Tools::debug(logBuffer);								
								
								// Solo envio este evento al Router, si no es el socket p2p, el que se cierra
								if (i != sock_p2p)
								{
									char *buffer_socket_id = new char[10];
									sprintf(buffer_socket_id, "%d", i);
											
									Event evRouter;
									evRouter.id = RT_RM_NODO_SERVIDOR;
									evRouter.tag =  Tools::duplicate(buffer_socket_id);
									Router::instance()->post_event(evRouter, true);										
								}
							}
							else								
							{
								memset(logBuffer, 0 , sizeof(logBuffer));
						        sprintf(logBuffer, "Listener: client_connections_admin: El socket [%d] no esta en el mapa interno, por lo que es un vecino el que se va", i);
								Tools::debug(logBuffer);							
							}
												
							cant_clientes--;							                       					
                        } 
                        else 
                        {
							memset(logBuffer, 0 , sizeof(logBuffer));
    		                sprintf(logBuffer, "Listener: client_connections_admin: recv (socket %d)", i);
							Tools::error(logBuffer);							
                        }
																		
                        close(i); // bye!
                        FD_CLR(i, &master);     // remove from master set
   						socket_ip_map.erase(i); // Remuevo la ip del mapa de control interno
						
						// Si el sock_p2p no esta cerrada y el socket que se cierra es el sock_p2p
						if (sock_p2p != SOCK_ERRONEO && i == sock_p2p)
						{
    		                sprintf(logBuffer, "Se cerro la conexion directa");
							Tools::info(logBuffer);	
									
							// Se le avisa al Router que borre el socket_p2p
							Event evRouter;
							evRouter.id = RT_UNSET_SOCKET_P2P;
							Router::instance()->post_event(evRouter, true);								
							
							// Se borra el socket_p2p del Listener
							sock_p2p = SOCK_ERRONEO;
							
							sprintf(logBuffer, "Salgo del estado 'en operacion'");
							Tools::info(logBuffer);								
							en_operacion = false;
						}
                    } 
                    else 
                    {// we got some data from a client
						
						if (!en_operacion)
						{
							Event ev;
							ev.id = CLIENT_MSG;
							ev.tag = Tools::duplicate(echoBuffer);
							Logic::instance()->post_event(ev, true);
						}
						else
						{
							if (sock_p2p != SOCK_ERRONEO && i == sock_p2p)
							{
								sprintf(logBuffer, "Me encuentro 'en operacion', pero el mensaje proviene de la conexion directa, asi que lo tomo");
								Tools::info(logBuffer);
								
								Event ev;
								ev.id = CLIENT_MSG;
								ev.tag = Tools::duplicate(echoBuffer);
								Logic::instance()->post_event(ev, true);
							}
							else
							{
								//sprintf(logBuffer, "Se recibio un mensaje y se descarto porque me encuentro en operacion. Mensaje [%s]", echoBuffer);
								sprintf(logBuffer, "Me encuentro 'en operacion', se recibio un mensaje y no proviene de la conexion directa. Lo descarto");
								Tools::info(logBuffer);
							}
						}																			
                    }
                }
            }
        }			
	}
	Event ev;
	ev.id = LOOP;
	this->post_event(ev, true); 	
}

void Listener::add_socket(int id_socket)
{
	char logBuffer[BUFFER_SIZE];
	sprintf(logBuffer, "Listener: Se agrega al conjunto de sockets de escucha, uno de los vecinos con el socket_id [%d]", id_socket);
	Tools::debug(logBuffer);		
		
	// add id_socket to the master set	
    FD_SET(id_socket, &master);
	
	if ( fdmax < id_socket )  // Si el socket que asigne al master es mas alto 
	    fdmax = id_socket;    // que el maximo socket, piso el maximo
	
//	Event evRta;
//	evRta.id = LOOP;
//	this->post_event(evRta, true); 	
}

// TODO no implementado aun
void Listener::rm_socket(int id_socket)
{		
	//Event evRta;
	//evRta.id = LOOP;
	//this->post_event(evRta, true); 
}


void Listener::add_socket_p2p(int id_socket)
{
	char logBuffer[BUFFER_SIZE];
	sprintf(logBuffer, "Listener: add_socket_p2p: Se agrega al conjunto de sockets de escucha, el socket p2p [%d]", id_socket);
	Tools::debug(logBuffer);		
		
	// add id_socket to the master set	
    FD_SET(id_socket, &master);
	
	if (fdmax < id_socket)  // Si el socket que asigne al master es mas alto
	    fdmax = id_socket;    // que el maximo socket, piso el maximo
	
	sock_p2p = id_socket;
	
//	Event evRta;
//	evRta.id = LOOP;
//	this->post_event(evRta, true); 	
}

void Listener::rm_socket_p2p(int id_socket)
{		
	char logBuffer[BUFFER_SIZE];
	sprintf(logBuffer, "Listener: rm_socket_p2p: Se remueve del conjunto de sockets de escucha, el socket p2p");
	Tools::debug(logBuffer);		
			
    FD_CLR(id_socket, &master);     // remove from master set
	socket_ip_map.erase(id_socket); // Remuevo la ip del mapa de control interno	
}

void Listener::decode_handshake_msg(const char *msg)
{
	char logBuffer[BUFFER_SIZE];	
	if (msg != NULL)
	{
		sprintf(logBuffer, "Recibiendo handshake [%s]", msg);
		Tools::info(logBuffer);	
	}   
	else
	{
		Tools::error("Listener: decode_handshake_msg: Mensaje NULL");
	}	
}

char *Listener::get_rta_handshake_msg()
{
	char logBuffer[BUFFER_SIZE];
	sprintf(logBuffer, "Enviando respuesta handshake [%s]", nombre_nodo);
	Tools::info(logBuffer);			
	return nombre_nodo;	
}

void Listener::close_TCP_connections() 
{
	close(servSock);
}

