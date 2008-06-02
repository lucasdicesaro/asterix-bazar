//
// Class: Nodo
//
#include "nodo.h"
#include "tools.h"

///
/// Constructors / Destructor
///
Nodo::Nodo()
{
}

Nodo::~Nodo()
{
}

///
/// Load
///
void Nodo::load(xmlpp::Element* el)
{
	char logBuffer[BUFFER_SIZE];
	using namespace xmlpp;
	Nodo* nodo = this;
	
	Attribute* attr = NULL;
//	attr = el->get_attribute("pos_x");
//	nodo->x = atoi(attr->get_value().c_str());
	attr = el->get_attribute("name");
	nodo->name = attr->get_value();
	//memset(logBuffer, 0 , sizeof(logBuffer));
	//sprintf(logBuffer, "Nodo: name: [%s]", attr->get_value().c_str());
	//Tools::debug(logBuffer);	
}

///
/// to xml
///
void Nodo::to_xml(xmlpp::Element* parent)
{
	using namespace xmlpp;
	//char buffer[64];
	
	Nodo* nodo = this;
	Element* node = parent->add_child("nodo");

//	memset(buffer, 0, sizeof(buffer));
//	sprintf(buffer, "%i", nodo->x / 1000);
//	node->set_attribute("pos_x", buffer);
		
	node->set_attribute("name", nodo->name);
}

