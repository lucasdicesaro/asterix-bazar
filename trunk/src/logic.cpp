

#include "common/tools.h"
#include "common/types.h"
#include "logic.h"
#include "router.h"
#include <assert.h>

Logic* Logic::single_instance = NULL;
bool primeraVez = true;
/**
* Constructors
*/
Logic::Logic()
{
}

Logic::~Logic()
{
}

Logic* Logic::instance()
{
	if (!single_instance)
		single_instance = new Logic;
	return single_instance;
}

void Logic::on_event(const Event& ev)
{
	switch (ev.id)
	{
		case INIT:
			Tools::debug("Logic: on_event: INIT:");
			// TODO
			// Preparar el inicio del Logic			
			// Definir cuando inicia el look up, teniendo en cuenta que ya no se define un rol.
			// Puede ser vendedor y comprador a la vez.
			break;			
			
		case CLIENT_MSG:
			Tools::debug("Logic: on_event: CLIENT_MSG:");
			on_client_msg(ev.tag);
			break;			
			
		case BUILD_MSG:
			Tools::debug("Logic: on_event: BUILD_MSG:");
			Event evSend;
	    	evSend.id = SEND_MSG;
			evSend.tag = Tools::duplicate(build_msg());
	    	Router::instance()->post_event(evSend, true); 			
			break;						
			
		//case PRE_QUIT:
		//	Tools::debug("Logic: on_event: QUIT:");			
		//	break;	
			
		default:
			Tools::debug("Logic: on event: *UNKNOWN*.");
			break;
	}
}


std::string Logic::build_msg()
{
	//<event ticket="Pepito0" />
	//Tools::debug("Listener: armando mensaje de bienvenida al cliente");
	using namespace xmlpp;
	using namespace std;
	Document doc;
	Element* node = NULL;
	Element* root = doc.create_root_node(XML_WELCOME_ROOT_ELEMENT);

	// TODO sacar este hardcode
//	std::string id = Logic::instance()->add_client();
	
	std::string sockDesc("Pepito");
	root->set_attribute(XML_WELCOME_FIRST_ELEMENT, sockDesc);

	return doc.write_to_string();
}



/**
Client msg handler:
*/
void Logic::on_client_msg(const void* msg)
{
	using namespace xmlpp;
	using namespace std;
	assert(msg);

	DomParser parser;
	parser.parse_memory((const char*)msg);
	Document* doc = parser.get_document();
	Element* root = doc->get_root_node();
	
	//string client = get_attr_value(root, "client");
	string client = Tools::get_attr_value(root, XML_WELCOME_FIRST_ELEMENT);
	//int id = atoi(get_attr_value(root, "id").c_str());
	//string timestamp = get_attr_value(root, "timestamp");
	//Entity* e = (Entity*)clients[client];
	//if (e == NULL)
	//	return; //client was removed.
	

	if (primeraVez && client == "Pepito")
	{
		Tools::debug("Logic: Es la primera vez que se ejecuta");
		primeraVez = false;
		Event ev;
    	ev.id = BUILD_MSG;
    	//ev.tag = duplicate(msg);
    	this->post_event(ev, true);
	}
	else
	{
		Tools::debug("Logic: Es la segunda vez que se ejecuta y ultima");
	}
		
		
	// Verificar si tengo lo que pide, si es Look up
	// Verificar si el paquete es mio, si es un Replay
	// Analizar todos lo casos..
	// Si no me corresponde
	// Decrementar el hopcount
	// y enviar a mis vecinos
	
	
	
	int id = -1;
	//Process event:
	switch (id)
	{		
		case LOOKUP:
			Tools::debug("Logic: on_event: LOOKUP:");
			break;
			
		case REPLAY:
			Tools::debug("Logic: on_event: REPLAY:");
			break;
						
		case BUY:
			Tools::debug("Logic: on_event: BUY:");		
			break;
	}
}
