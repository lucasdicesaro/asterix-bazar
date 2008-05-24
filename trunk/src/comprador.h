#ifndef _COMPRADOR_H_
#define _COMPRADOR_H_

#include "common/runner.h"
#include "common/types.h"

class Comprador : public Runner
{
	public:
		/**
		Singleton
		*/
		static Comprador* instance();
		/**
		* Implementacion requerida por Runner
		*/
		virtual void on_event(const Event& ev);
		
		void procesar_init(const void *event_tag);
		void procesar_replay(const void *event_tag);
		void procesar_pre_quit(const void *event_tag);
		
	protected:
		Comprador();
		~Comprador();
		static Comprador* single_instance;				
};

#endif //_COMPRADOR_H_
