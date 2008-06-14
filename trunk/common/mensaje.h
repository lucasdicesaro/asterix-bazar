//
// Class: Mensaje
#ifndef _MENSAJE_H_
#define _MENSAJE_H_

#include "types.h"
#include "nodo.h"
#include <stdlib.h>
#include <iostream>
#include <list>
#include <libxml++/libxml++.h>
#include <time.h>
#include <sys/time.h>

/**
Mensaje.
*/
class Mensaje
{
	public:
		typedef std::list<Nodo*> Nodos;
		
	public:
		Mensaje();
		~Mensaje();
		 
		/**
		Retorna false si no pudo cargar.
		*/
		bool load_file(std::string path);
		bool load(std::string xml);
		 
		/**
		Libera el mapa y las entidades.
		*/
		void release();

		/**
		Retorna las entidades.
		*/
		Nodos& get_nodos();
		void set_nodos(Nodos& nodos);		
		
		/**
		Retorna la cantidad de entidades en el mapa.
		*/
		unsigned int count() const;
		
		/**
		Para los fanaticos de java, hehe :P.
		*/
		std::string to_string(bool formated = false);		
		
		std::string get_code() const;
		void set_code(std::string code);
		
		std::string get_product_name() const;
		void set_product_name(std::string product_name);
				
		std::string get_vendedor() const;		
		void set_vendedor(std::string vendedor);		
				
		int get_cantidad() const;
		void set_cantidad(int cantidad = -1);
		
		int get_hopcount() const;
		void set_hopcount(int hopcount = -1);
		
		int get_timestamp_seconds() const;		
		void set_timestamp_seconds(int timestamp_seconds);		
		
		int get_timestamp_microseconds() const;		
		void set_timestamp_microseconds(int timestamp_microseconds);		
		
		char *get_timestamp_converted() const;
		
		Mensaje *clone();
		std::string get_next_node_name();
		std::string get_creator_node_name();
		
	protected:
		void load_nodo(xmlpp::Element* );
		bool load(xmlpp::Document* doc);
	
	protected:
		Nodos nodos;
		std::string code, product_name, vendedor;
		int cantidad, hopcount;
		int timestamp_seconds;
		int timestamp_microseconds;		
		struct timeval tv;
};


#endif	//_MENSAJE_H_

