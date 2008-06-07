//
// Class: Mensaje
//
#include "mensaje.h"
//#include "common/tools.h"
#include "tools.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>


/**
Constructores.
*/
Mensaje::Mensaje()
{
}
Mensaje::~Mensaje()
{
	release();
}


/**
To string
*/
std::string Mensaje::to_string(bool formated /*= false*/)
{
	using namespace xmlpp;
	Document doc;
	Element* node = NULL;
	char buffer[64];
	Element* root = doc.create_root_node("mensaje");
	assert(root);
	
	//width:
	node = root->add_child("body");
	
	//memset(buffer, 0, sizeof(buffer));
	//sprintf(buffer, "%i", this->width);
	//node->add_child_text(buffer);
	
	if (!this->code.empty())
	{
		node->set_attribute("code", this->code);
	}	
	if (!this->product_name.empty())
	{
		node->set_attribute("product_name", this->product_name);
	}	
	if (!this->vendedor.empty())
	{
		node->set_attribute("vendedor", this->vendedor);
	}	
	
	if (this->cantidad > 0)
	{
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "%i",this->cantidad);
		node->set_attribute("cantidad", buffer);	
	}

	if (this->hopcount > 0)
	{
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "%i",this->hopcount);
		node->set_attribute("hopcount", buffer);	
	}
		
	//entities:
	for (Nodos::iterator it = nodos.begin(); it != nodos.end(); it++)
	{
		Nodo* nodo = *it;
		nodo->to_xml(root);
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
void Mensaje::load_nodo(xmlpp::Element* el)
{
	using namespace xmlpp;
	Nodo* nodo = new Nodo;
	nodo->load(el);
	nodos.push_back(nodo);
	Tools::debug("Mensaje: load_nodo.");
}

/**
Load
*/
bool Mensaje::load_file(std::string path)
{
	using namespace xmlpp;
	DomParser parser(path, false);
	Document* doc = parser.get_document();
	return load(doc);
}

bool Mensaje::load(std::string content)
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
		Tools::debug("Mensaje: load: exception on load.");
		return false;
	}
}

bool Mensaje::load(xmlpp::Document* doc)
{
	char logBuffer[BUFFER_SIZE];
	
	using namespace xmlpp;
	release();
	Tools::debug("Mensaje: load.");

	Element* root = doc->get_root_node();
	assert(root);
	
	NodeSet nodes = root->find("/mensaje");
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

		//memset(logBuffer, 0 , sizeof(logBuffer));
		//sprintf(logBuffer, "Mensaje: node_name: [%s]", node_name.c_str());
		//Tools::debug(logBuffer);
		
		if (node_name == "body")
		{
			Attribute* attr = NULL;
			
			attr = el->get_attribute("code");
			this->code = attr->get_value();
			
			attr = el->get_attribute("product_name");
			if (attr != NULL)
			{
				this->product_name = attr->get_value();
				//memset(logBuffer, 0 , sizeof(logBuffer));
				//sprintf(logBuffer, "Mensaje: product_name: [%s]", attr->get_value().c_str());
				//Tools::debug(logBuffer);							
			}			
			attr = el->get_attribute("vendedor");
			if (attr != NULL)
			{
				this->vendedor = attr->get_value();			
				//memset(logBuffer, 0 , sizeof(logBuffer));
				//sprintf(logBuffer, "Mensaje: vendedor: [%s]", attr->get_value().c_str());
				//Tools::debug(logBuffer);			
			}					
			attr = el->get_attribute("cantidad");
			if (attr != NULL)
			{
				this->cantidad = atoi(attr->get_value().c_str());
			}
			attr = el->get_attribute("hopcount");
			if (attr != NULL)
			{
				this->hopcount = atoi(attr->get_value().c_str());	
			}
		}
		else if (node_name == "nodo")
		{
			load_nodo(el);
		}
		else if (node_name == "timestamp" && text)
		{
			// Si se necesita declarar el atributo "timestamp"
			//timestamp = atoi(text);
		}		
	}
	
	/*Tools::debug("Valores al finalizar");	
	memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Mensaje: this->code: [%s]", this->code.c_str());	
	Tools::debug(logBuffer);	
	memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Mensaje: this->product_name: [%s]", this->product_name.c_str());	
	Tools::debug(logBuffer);	
	memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Mensaje: this->cantidad: [%d]", this->cantidad);	
	Tools::debug(logBuffer);	
	memset(logBuffer, 0 , sizeof(logBuffer));
	sprintf(logBuffer, "Mensaje: this->hopcount: [%d]", this->hopcount);	
	Tools::debug(logBuffer);
	*/
	Tools::debug("Mapa: load complete.");
}
void Mensaje::release()
{
	if (nodos.size() > 0)
	{
		for (Nodos::iterator it = nodos.begin(); it != nodos.end(); it++)
			delete *it;
		nodos.clear();
	}
}

/**
Mensaje::operator[]
*/
Mensaje::Nodos& Mensaje::get_nodos()
{
	return nodos;
}
void Mensaje::set_nodos(Mensaje::Nodos& nodos)
{
	this->nodos = nodos;
}



std::string Mensaje::get_code() const
{
	return code;
}
std::string Mensaje::get_product_name() const
{
	return product_name;
}
std::string Mensaje::get_vendedor() const
{
	return vendedor;
}
int Mensaje::get_cantidad() const
{
	return cantidad;
}
int Mensaje::get_hopcount() const
{
	return hopcount;
}

void Mensaje::set_code(std::string code)
{
	this->code = code;
}
void Mensaje::set_product_name(std::string product_name)
{
	this->product_name = product_name;
}
void Mensaje::set_vendedor(std::string vendedor)
{
	this->vendedor = vendedor;
}
void Mensaje::set_cantidad(int cantidad)
{
	this->cantidad = cantidad;
}
void Mensaje::set_hopcount(int hopcount)
{
	this->hopcount = hopcount;
}

unsigned int Mensaje::count() const
{
	return nodos.size();
}

Mensaje *Mensaje::clone()
{
	Mensaje *mensaje = new Mensaje();
	Nodos nodos_aux;
	
	//if (!this->code.empty())
		mensaje->code = Tools::duplicate (this->code);
	//if (!this->product_name.empty())	
		mensaje->product_name = Tools::duplicate (this->product_name);	
	//if (!this->vendedor.empty())	
		mensaje->vendedor = Tools::duplicate (this->vendedor);			
	//if (this->cantidad > 0)
		mensaje->cantidad = this->cantidad;		
	//if (this->hopcount > 0)
		mensaje->hopcount = this->hopcount;				
	
	for (Nodos::iterator it = nodos.begin(); it != nodos.end(); it++)
	{
		Nodo* nodo = *it;
		nodos_aux.push_back(nodo->clone());
	}
	mensaje->nodos = nodos_aux;
	return mensaje;
}

std::string Mensaje::get_next_node_name()
{
	char logBuffer[BUFFER_SIZE];
	memset(logBuffer, 0 , sizeof(logBuffer));
	//Mensaje *mensaje = (Mensaje*)msg;	
	Nodo *nodo = NULL;
	std::string node_name;
	if (!this->get_nodos().empty())
	{
		nodo = (Nodo *)this->get_nodos().back();		
		if (nodo != NULL)
		{
			node_name = Tools::duplicate(nodo->name);
			sprintf(logBuffer, "Mensaje: get_next_node_name: nombre de nodo a retornar [%s]", node_name.c_str());
			Tools::debug(logBuffer);			
		}
		else
		{
			Tools::debug("Mensaje: get_next_node_name: nodo es NULL");
		}		
	}
	else
	{
		Tools::debug("Mensaje: get_next_node_name: mensaje->get_nodos() esta vacia");
	}
	return node_name;
}

std::string Mensaje::get_creator_node_name()
{
	char logBuffer[BUFFER_SIZE];
	memset(logBuffer, 0 , sizeof(logBuffer));
	
	std::string node_name;
	if (!nodos.empty())
	{
		Nodo *nodo = (Nodo *)nodos.front();
		if (nodo != NULL)
		{
			node_name = Tools::duplicate(nodo->name);
			sprintf(logBuffer, "Mensaje: get_creator_node_name: nombre de nodo a retornar [%s]", node_name.c_str());
			Tools::debug(logBuffer);			
		}
		else
		{
			Tools::debug("Mensaje: get_creator_node_name: nodo es NULL");
		}		
	}
	else
	{
		Tools::debug("Mensaje: get_creator_node_name: mensaje->get_nodos() esta vacia");
	}
	return node_name;
}

