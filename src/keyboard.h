#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "common/runner.h"

class Keyboard : public Runner
{
	public:
		/**
		Singleton
		*/
		static Keyboard* instance();
		/**
		* Implementacion requerida por Runner
		*/
		virtual void on_event(const Event& ev);

		int get_key();	

	protected:
		Keyboard();
		~Keyboard();
		static Keyboard* single_instance;				
};

#endif //_KEYBOARD_H_
