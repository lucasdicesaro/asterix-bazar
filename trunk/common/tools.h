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
		Imprimi una linea de debug
		*/
		void static debug(const char* );

		char* duplicate(const std::string& str);

		/**
		Parsea el archivo de configuracion dado y
		retorna 0 si OK, o != 0 si hay error.
		*/
		int Config_Parser (const char* FileName);
		
		/**
		Obtengo un participante
		*/
		char *get_IP_De_Participante(const char *nombre);

		/**
		CreateTCPServerSocket
		*/
		int  CreateTCPServerSocket(unsigned short port, int max_pending);

		/**
		AcceptTCPConnection
		*/
		ConnectDS* AcceptTCPConnection(int servSock);

		/**
		Imprime Mensaje con codigo de error y termina el proceso.
		*/
		void DieWithError(const char *errorMessage);

		void HandleTCPClient(int clntSocket);

		/**
		Retorna el valor del atributo o un string vacio si no existe:
		*/
		std::string get_attr_value(xmlpp::Element* el, const char* attr_name);

		/**
		Log rutines:
		*/
		int Open_Log (char* FileName);
		void Write_Log (char* strData);
		void Close_Log ();
		
	protected:
		/**
		Log rutines:
		*/
		FILE* LOG_FILE;
		ListaConfig listaConfig;
};

#endif //_TOOLS_H_
