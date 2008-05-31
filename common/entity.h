//
// Class: Entity
#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <libxml++/libxml++.h>

class Entity
{
public:
		Entity();
		//Entity(const Entity& e);
		 ~Entity();
		 
		 /**
		 Load
		 */
		 void load(xmlpp::Element* el);
		 
		 /**
		 To xml
		 */
		void to_xml(xmlpp::Element* parent);
		
	
//protected:
public:
	std::string name;

};


#endif	//_ENTITY_H_
