//
// Class: Mapa
//
#include "mapa.h"
#include "common/tools.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#define MX (1000)

/**
Constructores.
*/
Mapa::Mapa()
{
}
Mapa::~Mapa()
{
	release();
}


/**
To string
*/
std::string Mapa::to_string(bool formated /*= false*/)
{
	using namespace xmlpp;
	Document doc;
	Element* node = NULL;
	char buffer[64];
	Element* root = doc.create_root_node("mapa");
	assert(root);
	
	//width:
	node = root->add_child("body");
	
	//memset(buffer, 0, sizeof(buffer));
	//sprintf(buffer, "%i", this->width);
	//node->add_child_text(buffer);
	
	node->set_attribute("code", this->code);
	
	node->set_attribute("product_name", this->product_name);	
	
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%i",this->cantidad);
	node->set_attribute("cantidad", buffer);	

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%i",this->hopcount);
	node->set_attribute("hopcount", buffer);	
		
	//entities:
	for (Entities::iterator it = entities.begin(); it != entities.end(); it++)
	{
		Entity* entity = *it;
		entity->to_xml(root);
	}
	
	//timestamp:
	time_t t = time(NULL);
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%i", t);
	node = root->add_child("timestamp");
	node->add_child_text(buffer);
	
	return (formated)? doc.write_to_string_formatted() : doc.write_to_string();
}

/**
Load entity.
*/
void Mapa::load_entity(xmlpp::Element* el)
{
	using namespace xmlpp;
	Entity* en = new Entity;
	en->load(el);
	entities.push_back(en);
	Tools::debug("Mapa: load_entity.");
}

/**
Load
*/
bool Mapa::load_file(std::string path)
{
	using namespace xmlpp;
	DomParser parser(path, false);
	Document* doc = parser.get_document();
	return load(doc);
}

bool Mapa::load(std::string content)
{
	try
	{
		using namespace xmlpp;
		DomParser parser;
		parser.parse_memory(content);
		Document* doc = parser.get_document();
		return load(doc);
	} catch (...)
	{
		Tools::debug("Mapa: load: exception on load.");
		return false;
	}
}

bool Mapa::load(xmlpp::Document* doc)
{
	using namespace xmlpp;
	release();
	Tools::debug("Mapa: load.");

	Element* root = doc->get_root_node();
	assert(root);
	
	NodeSet nodes = root->find("/mapa");
	assert(!nodes.empty());
	Node* node = nodes[0];
	Node::NodeList childs = node->get_children();

	for (Node::NodeList::iterator it = childs.begin(); it != childs.end(); it++) 
	{
		Element* el = dynamic_cast<Element*>(*it);
		if (!el)
			continue;
		std::string node_name = el->get_name().c_str();
		ContentNode* ct = dynamic_cast<ContentNode*>(el->get_child_text());
		const char* text = (ct == NULL)? NULL : ct->get_content().c_str();

		if (node_name == "body" && text)
		{
			Attribute* attr = NULL;
			
			attr = el->get_attribute("code");
			this->code = attr->get_value();			
			
			attr = el->get_attribute("product_name");
			this->product_name = attr->get_value();			
			
			attr = el->get_attribute("cantidad");
			this->cantidad = atoi(attr->get_value().c_str());			

			attr = el->get_attribute("hopcount");
			this->hopcount = atoi(attr->get_value().c_str());	
		}
		else if (node_name == "entity")
		{
			load_entity(el);
		}
	}
	
	Tools::debug("Mapa: load complete.");
}
void Mapa::release()
{
	if (entities.size() > 0)
	{
		for (Entities::iterator it = entities.begin(); it != entities.end(); it++)
			delete *it;
		entities.clear();
	}
}

/**
Mapa::operator[]
*/
Mapa::Entities& Mapa::get_entities()
{
	return entities;
}


/**
Mapa::get_height
*/
/*const size_t Mapa::get_height() const
{
	return height;
}*/
/**
Mapa::get_width
*/
/*const size_t Mapa::get_width() const
{
	return width;
}*/
unsigned int Mapa::count() const
{
	return entities.size();
}

