#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "common/runner.h"
#include "common/tools.h"
#include "common/types.h"


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

	protected:		
		void get_key();	
		void procesar_submenu1();		
		void procesar_lanzar();
		void showstock();
		int elegir_producto();

	protected:	
	
		Keyboard();
		~Keyboard();
		static Keyboard* single_instance;				
};

#endif //_KEYBOARD_H_
