

#include "common/tools.h"
#include "common/types.h"
//#include "common/socket_util.h"
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
//#include <pthread.h>
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
			
		//case PRE_QUIT:
		//	Tools::debug("Listener: on_event: QUIT:");			
		//	break;	
			
		default:
			Tools::debug("Listener: on event: *UNKNOWN*.");
			break;
	}
}

std::string Listener::get_connect_msg(int socketId)
{
	//Tools::debug("Listener: armando mensaje de bienvenida al cliente");
	using namespace xmlpp;
	using namespace std;
	Document doc;
	Element* node = NULL;
	Element* root = doc.create_root_node(XML_WELCOME_ROOT_ELEMENT);

	// TODO sacar este hardcode
//	std::string id = Logic::instance()->add_client();
	
	stringstream my_stream;
	my_stream << socketId << flush;  	
	std::string sockId = my_stream.str();
	std::string sockDesc("Socket");
	root->set_attribute(XML_WELCOME_FIRST_ELEMENT, sockId + sockDesc);
	
	//char buffer[64];
	//time_t t = time(NULL);
	//memset(buffer, 0, sizeof(buffer));
	//sprintf(buffer, "%i", t);
	//root->set_attribute("timestamp", buffer);

	return doc.write_to_string();
}



void Listener::client_connections_admin()
{
    fd_set master;   // master file descriptor list
    fd_set read_fds; // temp file descriptor list for select()
    struct sockaddr_in myaddr;     // server address
    struct sockaddr_in remoteaddr; // client address
    int fdmax;        // maximum file descriptor number
    int newfd;        // newly accept()ed socket descriptor
    char buf[256];    // buffer for client data
    int nbytes;			//Size of received message
    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    socklen_t addrlen;
    int i, j;
	int cant_clientes = 0;
    
	char logBuffer[BUFFER_SIZE];        // Buffer for echo string 
    char echoBuffer[BUFFER_SIZE];        // Buffer for echo string 

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // get the servSock
    if ((servSock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        Tools::error("Listener: socket");
        exit(1);
    }

    // lose the pesky "address already in use" error message
    if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        Tools::error("Listener: setsockopt");
        exit(1);
    }

    // bind
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = INADDR_ANY;
    //myaddr.sin_port = htons(PORT_TCP);
	myaddr.sin_port = htons(listener_port);
    memset(&(myaddr.sin_zero), '\0', 8);
    if (bind(servSock, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
        Tools::error("Listener: bind");
        exit(1);
    }

    // listen
    if (listen(servSock, 10) == -1) {
        Tools::error("Listener: listen");
        exit(1);
    }

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
						std::string ans = get_connect_msg(newfd);
						int size = ans.length()+1;
						memset(logBuffer, 0 , sizeof(logBuffer));
						sprintf(logBuffer, "Listener: Se va a enviar el handshake al cliente: [%s] de %d bytes\n", ans.c_str(), size);
						Tools::info(logBuffer);						
						if (send(newfd, ans.c_str(), size, 0) != size)
						{
							Tools::error("Listener: Fallo el envio del handshake.");
							return ;
						}                            
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
    		                sprintf(logBuffer, "Listener: Removiendo la IP  [%s]\n", socket_ip_map[i].c_str());
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
                    } 
                    else 
                    {// we got some data from a client
                                            
						memset(logBuffer, 0 , sizeof(logBuffer));
						sprintf(logBuffer, "Listener: Paquete recibido del socker %d: [%s]", i, echoBuffer);
						Tools::info(logBuffer);
												
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



void Listener::close_TCP_connections() 
{
	close(servSock);
}

