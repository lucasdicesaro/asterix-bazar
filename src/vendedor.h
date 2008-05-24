#ifndef _VENDENDOR_H_
#define _VENDENDOR_H_

#include "common/runner.h"
#include "common/types.h"

class Vendedor : public Runner
{
	public:
		/**
		Singleton
		*/
		static Vendedor* instance();
		/**
		* Implementacion requerida por Runner
		*/
		virtual void on_event(const Event& ev);
		
		void procesar_look_up(const void *event_tag);
		void procesar_buy(const void *event_tag);
		void procesar_pre_quit(const void *event_tag);
	
	protected:
		Vendedor();
		~Vendedor();
		static Vendedor* single_instance;				
};

#endif //_VENDENDOR_H_
