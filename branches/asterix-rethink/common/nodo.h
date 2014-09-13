//
// Class: Nodo
#ifndef _NODO_H_
#define _NODO_H_

#include <libxml++/libxml++.h>

class Nodo
{
	public:
		Nodo();
		~Nodo();
		 
		 /**
		 Load
		 */
		void load(xmlpp::Element* el);
		 
		 /**
		 To xml
		 */
		void to_xml(xmlpp::Element* parent);
		
		Nodo *clone();
	
//protected:
	public:
		std::string name;
};


#endif	//_NODO_H_
