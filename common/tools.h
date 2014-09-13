#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "types.h"
#include <libxml++/libxml++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
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

		char static *duplicate(const std::string& str);
		
		/**
		Obtengo un participante
		*/
		char *get_IP_De_Participante(const char *nombre);
		
		ConfigDS *get_info_nodo(const char *nombre);

		void DieWithError(const char *errorMessage);

		/**
		Retorna el valor del atributo o un string vacio si no existe:
		*/
		std::string static get_attr_value(xmlpp::Element* el, const char* attr_name);

		ReconnectParamsDS *get_reconnect_params();
		void set_reconnect_params(ReconnectParamsDS *reconnectParamsDS);
		CurrentStockDS *get_current_stock();		
		void set_current_stock(CurrentStockDS *currentStockDS);

		void set_lista_config(std::list<ConfigDS> listaConfig); 

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
		CurrentStockDS *currentStock;

};

#endif //_TOOLS_H_

