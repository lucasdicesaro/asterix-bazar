//
// Class: Mapa
#ifndef _MAPA_H_
#define _MAPA_H_

#include "types.h"
#include "entity.h"
#include <stdlib.h>
#include <iostream>
#include <list>
#include <libxml++/libxml++.h>

/**
Mapa.
*/
class Mapa
{
	public:
		typedef std::list<Entity*> Entities;
		
	public:
		Mapa();
		 ~Mapa();
		 
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
		Entities& get_entities();
		
		/**
		Retorna la cantidad de entidades en el mapa.
		*/
		unsigned int count() const;
		
		/**
		Para los fanaticos de java, hehe :P.
		*/
		std::string to_string(bool formated = false);		
		 
	protected:
		void load_entity(xmlpp::Element* );
		bool load(xmlpp::Document* doc);
	
	protected:
		Entities entities;
		std::string code, product_name;
		int cantidad, hopcount;
};


#endif	//_MAPA_H_

