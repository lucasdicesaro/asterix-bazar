

#include "common/tools.h"
#include "common/types.h"
#include "common/socket_util.h"
#include "router.h"
#include <iostream>
#include <assert.h>

extern char *nombre_nodo;
extern int listener_port;

Router* Router::single_instance = NULL;

/**
* Constructors
*/
Router::Router()
{
}

Router::~Router()
{
}

Router* Router::instance()
{
	if (!single_instance)
		single_instance = new Router;
	return single_instance;
}

void Router::on_event(const Event& ev)
{
	switch (ev.id)
	{
		case INIT:
			Tools::debug("Router: on_event: INIT:");
			procesar_init(ev.tag);
			break;
			
		//case START_CONNECTION:
		//	Tools::debug("Router: on_event: START_CONNECTION:");
		//	procesar_start_connection(ev.tag);
		//	break;
			
		case WAIT_CONNECTIONS:
			Tools::debug("Router: on_event: WAIT_CONNECTIONS:");
			procesar_wait_connections(ev.tag);
			break;
			
		case TURNO_VENDEDOR:
			Tools::debug("Router: on_event: TURNO_VENDEDOR:");
			break;				
			
		case LOOKUP:
			Tools::debug("Router: on_event: LOOKUP:");
			//procesar_look_up(ev.tag);
			break;
			
		case REPLAY:
			Tools::debug("Vendedor: on_event: REPLAY:");
			break;
						
		case BUY:
			Tools::debug("Router: on_event: BUY:");
			//procesar_buy(ev.tag);			
			break;	
			
		case PRE_QUIT:
			Tools::debug("Router: on_event: QUIT:");
			procesar_pre_quit(ev.tag);
			break;	
			
		default:
			Tools::debug("Router: on event: *UNKNOWN*.");
			break;
	}
}

void Router::procesar_init(const void *event_tag) 
{	
	char echoBuffer[BUFFER_SIZE];		/* Buffer for echo string */
    int bytesRcvd;						/* Bytes read in single recv() */	
	
	Tools tools;
	tools.Config_Parser("lista_participante.conf");
	Tools::debug (nombre_nodo);
	ConfigDS* my_config = tools.get_info_nodo(nombre_nodo);
	//printf("my_config: nombre: [%s], ip: [%s], puerto: [%d]\n", my_config->nombre, my_config->ip, my_config->port);	
	
	ConfigDS* vecino1_config = tools.get_info_nodo(my_config->vecino1);	
	//printf("vecino1_config: nombre: [%s], ip: [%s], puerto: [%d]\n", vecino1_config->nombre, vecino1_config->ip, vecino1_config->port);
	if (vecino1_config == NULL) 
	{
		printf("El nodo %s, no existe en el archivo de configuracion\n", my_config->vecino1);
		exit(1);
	}
	
	ConfigDS* vecino2_config = tools.get_info_nodo(my_config->vecino2);
	if (vecino2_config == NULL) 
	{
		printf("El nombre de nodo %s, no existe en el archivo de configuracion\n", my_config->vecino2);
		exit(1);
	}
	
	/*char *buff = new char[10];
	memset(buff, 0, 30);
	memcpy(buff, &(vecino1_config->port), 10);
	Tools::debug (buff);*/
	
	sock_vecino1 = SocketUtil::cliente_abrir_conexion_tcp (vecino1_config->ip, vecino1_config->port);
	if (sock_vecino1 != SOCK_ERRONEO)
	{	
		memset(echoBuffer, 0, sizeof(echoBuffer));
		if ((bytesRcvd = recv(sock_vecino1, echoBuffer, BUFFER_SIZE, 0)) < 0)
		{
			perror("recv() failed or connection closed prematurely: vecino1");
			close(sock_vecino1);
			exit(1);
		}
		decode_mesage(echoBuffer);
	}
	
	sock_vecino2 = SocketUtil::cliente_abrir_conexion_tcp (vecino2_config->ip, vecino2_config->port);	
	if (sock_vecino2 != SOCK_ERRONEO)
	{		
		memset(echoBuffer, 0, sizeof(echoBuffer));
		if ((bytesRcvd = recv(sock_vecino2, echoBuffer, BUFFER_SIZE, 0)) < 0)
		{
			perror("recv() failed or connection closed prematurely: vecino2");
			close(sock_vecino2);
			exit(1);
		}
		decode_mesage(echoBuffer);
	}
	
	//const char *nodos = config.vecino1;
	int *port;
	*port = listener_port; // listener_port
	
	Event ev;
	ev.id  = WAIT_CONNECTIONS;
	ev.tag = port; 
	this->post_event (ev, true);
}

void Router::procesar_start_connection(const void *event_tag) 
{
	/*char *nodos = (char *)event_tag;
	Tools::debug (nodos);
	vecino1 = strtok(nodos, ",");
	Tools::debug(vecino1);
	vecino2 = strtok(NULL, ",");
	Tools::debug(vecino2);
	
	Tools tools;
	tools.Config_Parser("lista_participante.conf");

	char *dato_vecino1 = tools.get_IP_De_Participante(vecino1);
	Tools::debug(dato_vecino1);
	char *dato_vecino2 = tools.get_IP_De_Participante(vecino2);
	Tools::debug(dato_vecino2);
	
	char *ip_vecino1 = strtok(dato_vecino1, ",");
	Tools::debug(ip_vecino1);
	char *port_vecino1 = strtok(NULL, ",");
	Tools::debug(port_vecino1);
	sock_vecino1 = SocketUtil::cliente_abrir_conexion_tcp (ip_vecino1, atoi(port_vecino1));
	
	char *ip_vecino2 = strtok(dato_vecino2, ",");
	Tools::debug(ip_vecino2);
	char *port_vecino2 = strtok(NULL, ",");
	Tools::debug(port_vecino2);	
	sock_vecino2 = SocketUtil::cliente_abrir_conexion_tcp (ip_vecino2, atoi(port_vecino2));	
	
	int *port;
	*port = listener_port; // listener_port
	
	Event ev;
	ev.id  = WAIT_CONNECTIONS;
	ev.tag = port; 
	this->post_event (ev, true);*/
}



void Router::procesar_wait_connections(const void *event_tag) 
{
	int *listener_port = (int *)event_tag;
	char *buff = new char[10];
	memset(buff, 0, 30);
	memcpy(buff, listener_port, 10);
	Tools::debug(buff);
	
    fd_set master;   // master file descriptor list
    fd_set read_fds; // temp file descriptor list for select()
    struct sockaddr_in myaddr;     // server address
    struct sockaddr_in remoteaddr; // client address
    int fdmax;        // maximum file descriptor number
    int newfd;        // newly accept()ed socket 
    int nbytes;			//Size of received message
    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    socklen_t addrlen;
    int i, j;
	int numClientes = 0;
	char echoBuffer[BUFFER_SIZE];


	if ((servSock = SocketUtil::servidor_abrir_conexion_tcp(*listener_port)) < 0)
	{
		printf ("No se puede abrir socket servidor\n");
		exit (-1);
	}
	printf ("Se estableció el socket de servidor %d\n", servSock);
	
	
	

	FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);	
   // add the servSock to the master set
    FD_SET(servSock, &master);

    // keep track of the biggest file descriptor
    fdmax = servSock; // so far, it's this one

    // main loop

    while(true) 
    {
    
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
        {
            perror("Listener: select");
            return;
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) 
        {
	        if (FD_ISSET(i, &read_fds)) 
	        { // we got one!!
	        
                if (i == servSock) 
                {
                    // handle new connections
                    if (numClientes < MAX_CONNECTED)
                    {
                    addrlen = sizeof(remoteaddr);
                    if ((newfd = accept(servSock, (struct sockaddr *)&remoteaddr, &addrlen)) == -1) 
                    { 
                        perror("Listener: accept");
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
						
						//Event broad_ev;
						//broad_ev.id = BD_ADD_IP;
						//broad_ev.tag = ip;
						//Broadcaster::instance()->post_event(broad_ev, true);                        
						numClientes++;
//                      printf("selectserver: new connection from %s on socket %d\n", inet_ntoa(remoteaddr.sin_addr), newfd);
                                                                                   
						//handshake:
						std::string ans = get_connect_msg();
						int size = ans.length()+1;
//                      printf("Se va a enviar %s de %d bytes\n", ans.c_str(), size);
						if (send(newfd, ans.c_str(), size, 0) != size)
						{
							perror("Listener: Fallo el envio del handshake.");
							return ;
						}                            
                    }
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
                            printf("Listener: Connection closed (socket %d)\n", i);
                            //printf("Listener: removiendo ip %s\n", socket_ip_map[i].c_str()	);
					    	//Event evRmClient;
					    	//evRmClient.id = BD_RM_IP;
					    	//evRmClient.tag = duplicate(socket_ip_map[i]);
					    	//Broadcaster::instance()->post_event(evRmClient, true);                            
							numClientes--;
                        } 
                        else 
                        {
                            perror("Listener: recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
   						socket_ip_map.erase(i); //Remuevo la ip del mapa de control interno
                    } 
                    else 
                    {// we got some data from a client
                                            
                   		printf("Recobido paquete socket %d. Mensaje %s\n", i, echoBuffer);
			
				    	//Event ev;
				    	//ev.id = CLIENT_MSG;
				    	//ev.tag = duplicate(echoBuffer);
				    	//Logic::instance()->post_event(ev, true);                        
                    }
                } 
            }
        }
    }
    
	
	
	Event ev;
	ev.id  = PRE_QUIT;
	this->post_event (ev, true);
}

std::string Router::get_connect_msg()
{
	using namespace xmlpp;
	Document doc;
	Element* node = NULL;
	Element* root = doc.create_root_node("event");

	//std::string id = Logic::instance()->add_client();
	
	root->set_attribute("client", nombre_nodo);
	
	char buffer[64];
	time_t t = time(NULL);
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%i", t);
	root->set_attribute("timestamp", buffer);

	return doc.write_to_string();
}



void Router::decode_mesage(char* buffer)
{
	//<event client="player0" timestamp="12341234" />
	using namespace xmlpp;
	Tools::debug("> Sender::decode_mesage");		
	time_t last_tms = 0;
		
	DomParser parser;
	parser.parse_memory(buffer);
	
	Document* doc = parser.get_document();
	Element* root = doc->get_root_node();
	assert(root);
	
	NodeSet nodes = root->find("/event");
	assert(!nodes.empty());
	Node* node = nodes[0];

	Element* elem = dynamic_cast<Element*>(node);
			
	std::string node_name = elem->get_name().c_str();
	elem->get_child_text();
	
	Attribute* attr = NULL;
	attr = elem->get_attribute("client");
	const char * client = attr->get_value().c_str();
	
	Tools::debug("El connect retorno el siguiente client");
	Tools::debug(client);	
	attr = elem->get_attribute("timestamp");
	time_t tms = atoi(attr->get_value().c_str());
	
	//Sender::instance()->playerId = client;
	//Receptor::instance()->tms = tms;
	
	Tools::debug("< Sender: decode_mesage");
}

void Router::procesar_pre_quit(const void *event_tag) 
{
	// Inicia la rutina de salida
	close(sock_vecino1);
	close(sock_vecino2);
	close(servSock);	
	
	Event ev;
	ev.id  = QUIT;
	this->post_event(ev, true);
}



