#include "keyboard.h"
#include "logic.h"


Keyboard* Keyboard::single_instance = NULL;

/**
* Constructors
*/
Keyboard::Keyboard()
{
}

Keyboard::~Keyboard()
{
}

Keyboard* Keyboard::instance()
{
	if (!single_instance)
		single_instance = new Keyboard;
	return single_instance;
}

void Keyboard::on_event(const Event& ev)
{
	int id_socket;
	std::string socket;
	switch (ev.id)
	{
		case INIT:
			Tools::debug("Keyboard: on_event: INIT:");
			break;
			
		default:
			Tools::debug("Keyboard: on event: *UNKNOWN*.");
			break;
	}
}

int Keyboard::get_key()
{
	char *buffer = new char[BUFFER_SIZE];
	
	Event ev;
	//ev.id = CLIENT_MSG;
	ev.tag = Tools::duplicate(buffer);
	Logic::instance()->post_event(ev, true);                        
}


