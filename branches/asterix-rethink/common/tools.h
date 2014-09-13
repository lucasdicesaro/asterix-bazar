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
		/**
		Singleton
		*/
		static Tools* instance();

		char static *duplicate(const std::string& str);
		
		void DieWithError(const char *errorMessage);

		/**
		Retorna el valor del atributo o un string vacio si no existe:
		*/
		std::string static get_attr_value(xmlpp::Element* el, const char* attr_name);

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
};

#endif //_TOOLS_H_

