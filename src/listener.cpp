

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

Listener* Listener::single_instance = NULL;

/**
* Constructors
*/
Listener::Listener()
{
	yes=1; // for setsockopt() SO_REUSEADDR, below	
	cant_clientes = 0;
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
//  while(true) 
//  {    
    timeout.tv_sec = 5;// Timeout del select
    timeout.tv_usec = 0;	
    read_fds = master; // copy it
	//Tools::debug("Listener: client_connections_admin: antes del select");	
	int retornoSelect;
	if ((retornoSelect = select(fdmax+1, &read_fds, NULL, NULL, &timeout)) == -1) 
    //if ((retornoSelect = select(fdmax+1, &read_fds, NULL, NULL, NULL)) == -1) 
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
                        sprintf(logBuffer, "Listener: Nueva conexion de [%s] con el socket %d", inet_ntoa(remoteaddr.sin_addr), newfd);
						Tools::debug(logBuffer);
						
						char *nombre_nodo_vecino = new char[BUFFER_SIZE];
						// Recibo el nombre de nodo de mi vecino
						memset(nombre_nodo_vecino, 0, BUFFER_SIZE);
						nombre_nodo_vecino = SocketUtil::recibir_mensaje(newfd);						
						decode_handshake_msg(nombre_nodo_vecino);
						
						// Armo el mensaje con mi nombre de nodo y lo mando a mi vecino
						char *rta_hndsk = get_rta_handshake_msg();
						SocketUtil::enviar_mensaje(newfd, rta_hndsk);
						
						
						// Transformo el socket de int a char*
						char *buffer_nodo_socket_id = new char[BUFFER_SIZE];
						memset(buffer_nodo_socket_id, 0, BUFFER_SIZE);
						sprintf(buffer_nodo_socket_id, "%s,%d", nombre_nodo_vecino, newfd);
							
						Event broad_ev;
						broad_ev.id = RT_ADD_NODO_SERVIDOR;
						broad_ev.tag = Tools::duplicate(buffer_nodo_socket_id);
						Router::instance()->post_event(broad_ev, true);						
                    }
                  }
				  else 
				  {
					  Tools::error("Listener: Se llego al limite de conexiones aceptadas por el Listener");
					  Tools::debug_label_value ("Listener: Maximo de clientes conectados", MAX_CONNECTED);
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
    		                sprintf(logBuffer, "Listener: Connection closed (socket %d)", i);
							Tools::debug(logBuffer);
							
							if (!socket_ip_map[i].empty()) 
							{
								memset(logBuffer, 0 , sizeof(logBuffer));
						        sprintf(logBuffer, "Listener: Removiendo la IP [%s]", socket_ip_map[i].c_str());
								Tools::debug(logBuffer);
								
								// Transformo el socket de int a char*
								char *buffer_socket_id = new char[10];
								sprintf(buffer_socket_id, "%d", i);
											
								Event evRmClient;
								evRmClient.id = RT_RM_NODO_SERVIDOR;
								evRmClient.tag =  Tools::duplicate(buffer_socket_id);
								Router::instance()->post_event(evRmClient, true);										
							}
							else								
							{
								memset(logBuffer, 0 , sizeof(logBuffer));
						        sprintf(logBuffer, "Listener: El socket [%d] no esta en el mapa interno, por lo que es un vecino el que se va", i);
								Tools::debug(logBuffer);							
							}
												
							cant_clientes--;							                       					
                        } 
                        else 
                        {
							memset(logBuffer, 0 , sizeof(logBuffer));
    		                sprintf(logBuffer, "Listener: recv (socket %d)", i);
							Tools::error(logBuffer);							
                        }
																		
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
   						socket_ip_map.erase(i); //Remuevo la ip del mapa de control interno
                    } 
                    else 
                    {// we got some data from a client
                                            
						//memset(logBuffer, 0 , sizeof(logBuffer));
						//sprintf(logBuffer, "Listener: Paquete recibido del socker %d: [%s]", i, echoBuffer);
						//Tools::debug(logBuffer);
												
				    	Event ev;
				    	ev.id = CLIENT_MSG;
						ev.tag = Tools::duplicate(echoBuffer);
				    	Logic::instance()->post_event(ev, true);                        
                    }
                }
            }
        }
			
	}			
//  }
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
	
	Event evRta;
	evRta.id = LOOP;
	this->post_event(evRta, true); 	
}

void Listener::rm_socket(int id_socket)
{		
	Event evRta;
	evRta.id = LOOP;
	this->post_event(evRta, true); 
}

void Listener::decode_handshake_msg(const char *msg)
{
	char logBuffer[BUFFER_SIZE];	
	if (msg != NULL)
	{
		sprintf(logBuffer, "Listener: Recibiendo handshake [%s]", msg);
		Tools::debug(logBuffer);	
	}   
	else
	{
		Tools::error("Listener: decode_handshake_msg: Mensaje NULL");
	}	
}

char *Listener::get_rta_handshake_msg()
{
	char logBuffer[BUFFER_SIZE];
	sprintf(logBuffer, "Listener: get_rta_handshake_msg: Enviando respuesta handshake [%s]", nombre_nodo);
	Tools::debug(logBuffer);			
	return nombre_nodo;	
}

void Listener::close_TCP_connections() 
{
	close(servSock);
}

