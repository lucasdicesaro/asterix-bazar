//
// Class: Entity
//
#include "entity.h"
#include "tools.h"

///
/// Constructors / Destructor
///
Entity::Entity()
{
}

Entity::~Entity()
{
}

///
/// Load
///
void Entity::load(xmlpp::Element* el)
{
	using namespace xmlpp;
	Entity* en = this;
	
	Attribute* attr = NULL;
//	attr = el->get_attribute("pos_x");
//	en->x = atoi(attr->get_value().c_str());
	attr = el->get_attribute("name");
	en->name = attr->get_value();
}

///
/// to xml
///
void Entity::to_xml(xmlpp::Element* parent)
{
	using namespace xmlpp;
	char buffer[64];
	
	Entity* entity = this;
	Element* node = parent->add_child("entity");

//	memset(buffer, 0, sizeof(buffer));
//	sprintf(buffer, "%i", entity->x / 1000);
//	node->set_attribute("pos_x", buffer);
		
	node->set_attribute("name", entity->name);
}

