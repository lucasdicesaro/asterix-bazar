#include "keyboard.h"
#include "common/tools.h"

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
}


