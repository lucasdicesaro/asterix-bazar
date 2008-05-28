#include "tools.h"
#include <iostream>
#include <assert.h>
#include <string.h>
#include <algorithm>  // For find
/**
Debug
*/
void Tools::debug(const char* msg)
{
	printf("> %s\n", msg);
}

/**
DieWithError
*/
void Tools::DieWithError(const char *errorMessage)
{
	perror(errorMessage);
	exit(0);
}

/**
duplicate
*/
char* Tools::duplicate(const std::string& str)
{
	int size = str.length()+1;
	char* buff = new char[size];
	memset(buff, 0, size);
	strcpy(buff, str.c_str());
	return buff;
}


/**
Get attribute value
*/
std::string Tools::get_attr_value(xmlpp::Element* el, const char* attr_name)
{
	using namespace xmlpp;
	assert(el);
	Attribute* attr = el->get_attribute(attr_name);
	return (attr == NULL)? "" : attr->get_value();
}

/**
AcceptTCPConnection
*/
ConnectDS* Tools::AcceptTCPConnection(int servSock)
{
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int clntLen;            /* Length of client address data structure */

    /* Set the size of the in-out parameter */
    clntLen = sizeof(echoClntAddr);
    
    /* Wait for a client to connect */
    if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
     DieWithError("accept() failed");
    
   char *client_ip = inet_ntoa(echoClntAddr.sin_addr);
    ConnectDS* ret = new ConnectDS;
    memset(ret->ip, 0, sizeof(ret->ip));
    ret->sock = clntSock;
    strcpy(ret->ip, client_ip);

    return ret;
}


/**
CreateTCPServerSocket
*/
int Tools::CreateTCPServerSocket(unsigned short port, int max_pending)
{
    int sock;                        /* socket to create */
    struct sockaddr_in echoServAddr; /* Local address */

    /* Create socket for incoming connections */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
      
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(port);              /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(sock, max_pending) < 0)
        DieWithError("listen() failed");
      
    return sock;
}


/**
Config_Parser
*/
#define BUFFER_STR 128
int Tools::Config_Parser (const char* FileName)
{
	FILE *CONFIG_FILE;	ConfigDS conf;	
	char Line[BUFFER_STR];
	char* Param;
	char* Value;
	char* nombre;
	char* ip_port;
	char* ip;
	int port;
	char* vecino1;	
	char* vecino2;


	/* abro archivo 
	*/
	if ((CONFIG_FILE = fopen(FileName, "r")) == NULL) {
		DieWithError("No se pudo abrir archivo de configuracion");
		return NULL; 
	}

	/* Leo la primera linea 
    */
	fgets(Line, sizeof(Line),  CONFIG_FILE);


	/* Reemplazo el '\n' al final del registro por '\0'
    */
	Line[strlen(Line)-1] = '\0';


	/* Valido que el archivo  no este vacio
	*/
	if (feof(CONFIG_FILE)) {
		DieWithError("El archivo de configuracion esta vacio");
		return NULL; 
	}

	/* Recorro y parseo los registros
    */
	while (!feof(CONFIG_FILE))
	{
		Param = strtok(Line, "=");
		//printf ("Line: %s\n", Line);
		Value  = strtok(NULL , "=");
		
		//printf ("Param: %s - Value: %s\n", Param, Value);
		
		if (strcmp(Param, "nombre_nodo") == 0)
		{
			nombre_nodo = new char[NOMBRE_NODO_SIZE];
			memset(nombre_nodo, 0, NOMBRE_NODO_SIZE);
			strcpy(nombre_nodo,  Value);
		}

		if  (strcmp(Param, "listener_port") == 0)
		{
			listener_port = atoi(Value);
		}
		
		/*
		// PARAMETROS CHAR //
		if  (strcmp(Param, "param_1") == 0)
			 strcpy(conf->param_1,  Value);
			 
		// PARAMETROS INT //
		if  (strcmp(Param, "param_3") == 0) 	
			 conf->param_3 = atoi(Value);*/
		
		if  (strcmp(Param, "participante") == 0) {
			nombre = strtok(Value, ",");
			ip_port = strtok(NULL , ",");						
			vecino1 = strtok(NULL , ",");
			vecino2 = strtok(NULL , ",");

			ip = strtok(ip_port, ":");
			port = atoi(strtok(NULL , ":"));
			//printf ("ip: %s - port: %d\n", ip, port);
						
			strcpy(conf.nombre, nombre);
			strcpy(conf.ip, ip);			
			conf.port = port;
			strcpy(conf.vecino1, vecino1);
			strcpy(conf.vecino2, vecino2);
			listaConfig.push_back(conf);			
		}
		
		/*	Leo la siguiente linea 
    	*/
		fgets(Line, sizeof(Line), CONFIG_FILE);
		Line[strlen(Line)-1] = '\0';
	}

	/* Cierro el archivo 
	*/
	fclose(CONFIG_FILE);	
	
	return NULL; 
}

char *Tools::get_IP_De_Participante(const char *nombre) {
	
	char *key = duplicate(nombre);
	ConfigDS* elementoConfig = NULL;
	for (ListaConfig::iterator it = listaConfig.begin(); it != listaConfig.end() && elementoConfig == NULL; it++)
	{
		ConfigDS tmp = *it;
		if (strcmp(tmp.nombre, key) == 0) {
			elementoConfig = &tmp;
		}
	}	
	return duplicate (elementoConfig->ip);
}

ConfigDS *Tools::get_info_nodo(const char *nombre) {
	
	char *key = duplicate(nombre);
	//memset(key, 0, NOMBRE_NODO_SIZE);
	//strcpy(key, nombre);
	
	ConfigDS* elementoConfig = NULL;
	for (ListaConfig::iterator it = listaConfig.begin(); it != listaConfig.end() && elementoConfig == NULL; it++)
	{
		ConfigDS tmp = *it;
		//printf("tmp.nombre: {%s} - key: {%s} - strcm %d\n", tmp.nombre, key, strcmp(tmp.nombre, key));
		if (strcmp(tmp.nombre, key) == 0) {
			//printf("-> tmp.nombre: {%s}\n", tmp.nombre);			
			//elementoConfig->nombre = new char[]
			//memset(elementoConfig->nombre, 0, NOMBRE_NODO_SIZE);
			elementoConfig = new ConfigDS();
			strcpy(elementoConfig->nombre, tmp.nombre);
			strcpy(elementoConfig->ip, tmp.ip);
			elementoConfig->port = tmp.port;
			strcpy(elementoConfig->vecino1, tmp.vecino1);
			strcpy(elementoConfig->vecino2, tmp.vecino2);
			//elementoConfig = &tmp;
			//strcpy(elementoConfig->nombre, tmp.nombre);
			//strcpy(elementoConfig->ip, tmp.ip);
			//elementoConfig->port = tmp.port;
										   
			//strcpy(elementoConfig->ip = &(tmp.ip));			
			//printf("-> elementoConfig->nombre: {%s}\n", elementoConfig->nombre);
		}
	}	
	//printf("elementoConfig: nombre: [%s], ip: [%s], puerto: [%d]\n", elementoConfig->nombre, elementoConfig->ip, elementoConfig->port);
	//char*nombre_nodo = duplicate (elementoConfig->nombre);
	//printf("nombre_nodo: [%s] \n",nombre_nodo);
	//strcpy(elementoConfig->nombre, nombre_nodo);
	//printf("elementoConfig->nombre: [%s] \n",elementoConfig->nombre);	
	return elementoConfig;
}


/**
Abrir log
*/
int Tools::Open_Log (char* FileName)
{
	/* abro archivo de log
	*/
	if ((LOG_FILE = fopen(FileName, "ab")) == NULL)
		return(2);
	else
		return(0);	
}
/**
Escribir log
*/
void Tools::Write_Log (char* strData)
{
	char fechahora[23];
	time_t segundos;
	struct tm *fecha;

	char buffer[2048];        


	/* obtengo fecha y hora
	*/
	segundos = time(NULL);
	fecha = localtime(&segundos);
	strftime(fechahora, 24, "%Y-%m-%d %H:%M:%S -> ", fecha);

	/* preparo string a grabar
	*/
	buffer[0] = '\0';
	strcpy(buffer, fechahora);
	strcat(buffer, strData);
	strcat(buffer, "\n\0");

	/* grabo
	*/
	fwrite(buffer, 1, strlen(buffer), LOG_FILE);
	
	return;
}
/**
Cierro log
*/
void Tools::Close_Log ()
{
	/* cierro archivo de log
	*/
	fclose(LOG_FILE);

	return;
}

char * Tools::get_nombre_nodo() 
{
	return nombre_nodo;
}
int Tools::get_listener_port()
{
	return listener_port;
}

