#include "tools.h"
#include <iostream>
#include <assert.h>
#include <string.h>
#include <algorithm>  // For find
Tools* Tools::single_instance = NULL;



/**
* Constructors
*/
Tools::Tools()
{
}

Tools::~Tools()
{
}

Tools* Tools::instance()
{
	if (!single_instance)
		single_instance = new Tools;
	return single_instance;
}

/**
Debug
*/
void Tools::debug(const char* msg)
{
	if (Tools::instance()->debug_level)
		printf("%s %s\n", DEBUG_LEVEL, msg);
}

void Tools::debug(std::string msg)
{
	if (Tools::instance()->debug_level)	
		printf("%s %s\n", DEBUG_LEVEL, msg.c_str());	
}

void Tools::debug(const char* nombre_nodo, const char* msg)
{
	if (Tools::instance()->debug_level)	
		printf("%s - %s %s\n", nombre_nodo, DEBUG_LEVEL, msg);
}


void Tools::info(std::string msg)
{
	if (Tools::instance()->info_level)	
		printf("%s %s\n", INFO_LEVEL, msg.c_str());	
}

void Tools::warn(std::string msg)
{
	if (Tools::instance()->warn_level)	
		printf("%s %s\n", WARN_LEVEL, msg.c_str());	
}

void Tools::error(std::string msg)
{
	if (Tools::instance()->error_level)	
	{
		printf("%s %s\n", ERROR_LEVEL, msg.c_str());
		//perror(msg.c_str());	
	}
}

void Tools::debug_label_value(std::string label, int value)
{
	if (Tools::instance()->debug_level)
		std::cout << DEBUG_LEVEL << " " << label.c_str() << ": " << value << std::endl;
}

void Tools::info_label_value(std::string label, int value)
{
	if (Tools::instance()->info_level)	
		std::cout << INFO_LEVEL << " " << label.c_str() << ": " << value << std::endl;
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
	reconnectParams = new ReconnectParamsDS();


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

		//if  (strcmp(Param, "listener_port") == 0)
		//{
		//	listener_port = atoi(Value);
		//}		
		if  (strcmp(Param, "intentos_reconexion") == 0)
		{
			reconnectParams->intentos_reconexion = atoi(Value);
		}
		if  (strcmp(Param, "delay_reconexion") == 0)
		{
			reconnectParams->delay_reconexion = atoi(Value);
		}
		if  (strcmp(Param, "hopcount") == 0)
		{
			reconnectParams->hopcount = atoi(Value);
		}		
		
		if  (strcmp(Param, "participante") == 0) {
			nombre = strtok(Value, ",");
			ip_port = strtok(NULL , ",");						
			vecino1 = strtok(NULL , ",");
			vecino2 = strtok(NULL , ",");

			ip = strtok(ip_port, ":");
			port = atoi(strtok(NULL , ":"));
			//printf ("ip: %s - port: %d\n", ip, port);
			
			// Guardo el listener_port, si el nodo que estoy parseando soy yo
			if (strcmp(nombre, nombre_nodo) == 0)
			{
				listener_port = port;
			}
			
			strcpy(conf.nombre, nombre);
			strcpy(conf.ip, ip);			
			conf.port = port;
			strcpy(conf.vecino1, vecino1);
			strcpy(conf.vecino2, vecino2);
			listaConfig.push_back(conf);			
		}
		
		if  (strcmp(Param, DEBUG_LEVEL) == 0)
		{
			debug_level = atoi(Value);
		}		
		if  (strcmp(Param, INFO_LEVEL) == 0)
		{
			info_level = atoi(Value);
		}				
		if  (strcmp(Param, WARN_LEVEL) == 0)
		{
			warn_level = atoi(Value);
		}
		if  (strcmp(Param, ERROR_LEVEL) == 0)
		{
			error_level = atoi(Value);
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



char *Tools::get_nombre_nodo() 
{
	return nombre_nodo;
}

int Tools::get_listener_port()
{
	return listener_port;
}

ReconnectParamsDS *Tools::get_reconnect_params()
{
	return reconnectParams;
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

