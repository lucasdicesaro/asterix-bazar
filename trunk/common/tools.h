#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "types.h"
#include <libxml++/libxml++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <list>

class Tools
{
	public:
		typedef std::list<ConfigDS> ListaConfig;
		/**
		Singleton
		*/
		static Tools* instance();
		
		/**
		Imprimi una linea de debug
		*/
		void static debug(const char* msg);
		void static debug(const char* nombre_nodo, const char* msg);
		void static debug(std::string msg);
		void static info(std::string msg);
		void static warn(std::string msg);
		void static error(std::string msg);		
		void static debug_label_value(std::string label, int value);
		void static info_label_value(std::string label, int value);		

		char static *duplicate(const std::string& str);

		/**
		Parsea el archivo de configuracion dado y
		retorna 0 si OK, o != 0 si hay error.
		*/
		int Config_Parser (const char* FileName);
		
		/**
		Obtengo un participante
		*/
		char *get_IP_De_Participante(const char *nombre);
		
		ConfigDS *get_info_nodo(const char *nombre);
		ReconnectParamsDS *get_reconnect_params();

		void DieWithError(const char *errorMessage);

		/**
		Retorna el valor del atributo o un string vacio si no existe:
		*/
		std::string static get_attr_value(xmlpp::Element* el, const char* attr_name);
		
		char *get_nombre_nodo();
		int get_listener_port();
		
		/**
		Log rutines:
		*/
		int Open_Log (char* FileName);
		void Write_Log (char* strData);
		void Close_Log ();		
				
	protected:
		Tools();
		~Tools();
		static Tools* single_instance;
		/**
		Log rutines:
		*/
		FILE* LOG_FILE;
		ListaConfig listaConfig;
		ReconnectParamsDS *reconnectParams;
		char *nombre_nodo;
		int listener_port;	
		
		bool debug_level;
		bool info_level;
		bool warn_level;				
		bool error_level;						
};

#endif //_TOOLS_H_
