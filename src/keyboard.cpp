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
		  	this->get_key();
		  break;

		case TECLA:
			Tools::debug("Keyboard: on_event: TECLA:");
			break;

		default:
			Tools::debug("Keyboard: on event: *UNKNOWN*.");
			break;
	}
}

void Keyboard::get_key()
{
   	Event ev, ev1;
	char *buffer = new char[BUFFER_SIZE];
	int key;
   	
	key = getchar();   
   	ev.id = TECLA;
	sprintf(buffer,"%d", key);
	ev.tag = buffer;
	sprintf(buffer, "La tecla que apreto es %d", key);
   	Tools::debug(buffer);
	Logic::instance()->post_event(ev, true);
	ev1.id = INIT;
	Keyboard::instance()->post_event(ev1, true);

}


