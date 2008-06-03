

#include "common/tools.h"
#include "common/types.h"
#include "common/socket_util.h"
#include "listener.h"
#include "logic.h"
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
	switch (ev.id)
	{
		case INIT:
			Tools::debug("Listener: on_event: INIT:");
			client_connections_admin();
			break;
			
		default:
			Tools::debug("Listener: on event: *UNKNOWN*.");
			break;
	}
}


void Listener::client_connections_admin()
{
    fd_set master;   // master file descriptor list
    fd_set read_fds; // temp file descriptor list for select()
    //struct sockaddr_in myaddr;     // server address
    struct sockaddr_in remoteaddr; // client address
    int fdmax;        // maximum file descriptor number
    int newfd;        // newly accept()ed socket descriptor
    int nbytes;			//Size of received message
    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    socklen_t addrlen;
    int i, j;
	int cant_clientes = 0;
    
	char logBuffer[BUFFER_SIZE];        // Buffer for log
    char echoBuffer[BUFFER_SIZE];        // Buffer for echo string 
	char *handshakeBuffer = new char [BUFFER_SIZE]; // Buffer handshake

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);
	
	servSock = SocketUtil::servidor_abrir_conexion_tcp(listener_port);
	
    // add the servSock to the master set
    FD_SET(servSock, &master);

    // keep track of the biggest file descriptor
    fdmax = servSock; // so far, it's this one

    // main loop

    while(true) 
    {    
        read_fds = master; // copy it
		Tools::debug("Listener: antes del select");
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
        {
            Tools::error("Listener: select");
            return;
        }
		Tools::debug("Listener: despues del select");
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
						//Event broad_ev;
						//broad_ev.id = BD_ADD_IP;
						//broad_ev.tag = ip;
						//Broadcaster::instance()->post_event(broad_ev, true);                        
						memset(logBuffer, 0 , sizeof(logBuffer));
                        sprintf(logBuffer, "Listener: Nueva conexion de [%s] con el socket %d", inet_ntoa(remoteaddr.sin_addr), newfd);
						Tools::info(logBuffer);
						
						//handshake:						
						memset(handshakeBuffer, 0, BUFFER_SIZE);
						handshakeBuffer = SocketUtil::recibir_mensaje(newfd);						
						decode_mesage(handshakeBuffer);
						socket_nodo_map[newfd] = handshakeBuffer;
                    }
                  }
				  else 
				  {
					  Tools::error("Listener: Se llego al limite de conexiones aceptadas por el Listener");
					  Tools::info_label_value ("Listener: Maximo de clientes conectados", MAX_CONNECTED);
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
							Tools::info(logBuffer);
							
							memset(logBuffer, 0 , sizeof(logBuffer));
    		                sprintf(logBuffer, "Listener: Removiendo la IP [%s]\n", socket_ip_map[i].c_str());
							Tools::info(logBuffer);							
							cant_clientes--;
					    	//Event evRmClient;
					    	//evRmClient.id = BD_RM_IP;
					    	//evRmClient.tag = duplicate(socket_ip_map[i]);
					    	//Broadcaster::instance()->post_event(evRmClient, true);                            
                        } 
                        else 
                        {
                            Tools::error("Listener: recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
   						socket_ip_map.erase(i); //Remuevo la ip del mapa de control interno
						socket_nodo_map.erase(i); //Remuevo el nodo del mapa de control interno
                    } 
                    else 
                    {// we got some data from a client
                                            
						//memset(logBuffer, 0 , sizeof(logBuffer));
						//sprintf(logBuffer, "Listener: Paquete recibido del socker %d: [%s]", i, echoBuffer);
						//Tools::info(logBuffer);
												
				    	Event ev;
				    	ev.id = CLIENT_MSG;
						ev.tag = Tools::duplicate(echoBuffer);
				    	Logic::instance()->post_event(ev, true);                        
                    }
                }
            }
        }
    }
}



void Listener::decode_mesage(char* buffer)
{
	char logBuffer[BUFFER_SIZE];
	sprintf(logBuffer, "Listener: Recibiendo Handshake [%s]", buffer);
	Tools::info(logBuffer);	
}

int Listener::get_socket_from_client(std::string nombre_nodo)
{	
	char logBuffer[BUFFER_SIZE];
	std::string aux;
	int socket = SOCK_ERRONEO;	
	if (!nombre_nodo.empty())
	{
		//lock();
		memset(logBuffer, 0 , sizeof(logBuffer));
		sprintf(logBuffer, "Listener: get_socket_from_client: Nombre_nodo [%s]", nombre_nodo.c_str());
		Tools::debug(logBuffer);			
		bool encontrado = false;
		for (NodoMappingIterator it = socket_nodo_map.begin(); encontrado || it != socket_nodo_map.end(); it++)
		{
			aux = (*it).second;
			memset(logBuffer, 0 , sizeof(logBuffer));
			sprintf(logBuffer, "Listener: get_socket_from_client: Nodo actual [%s]", aux.c_str());
			Tools::debug(logBuffer);			
			if (nombre_nodo.compare(aux) == 0)
			{
				socket = (*it).first;
				encontrado = true;				
				memset(logBuffer, 0 , sizeof(logBuffer));
				sprintf(logBuffer, "Listener: get_socket_from_client: Socket [%d]", socket);
				Tools::debug(logBuffer);				
			}
		}
		//unlock();	
	}
	else 
	{
		Tools::error("Listener: get_socket_from_client: Nombre_nodo esta vacio");
	}
	return socket;
}


void Listener::close_TCP_connections() 
{
	close(servSock);
}

