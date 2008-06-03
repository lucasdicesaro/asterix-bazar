#ifndef _LOGIC_H_
#define _LOGIC_H_

#include "common/runner.h"
#include "common/mensaje.h"
#include "common/nodo.h"

class Logic : public Runner
{
	public:
		
		/**
		Singleton
		*/
		static Logic* instance();
		/**
		* Implementacion requerida por Runner
		*/
		virtual void on_event(const Event& ev);

		Mensaje *build_look_up_msg(std::string product_name, int cantidad);
		Mensaje *build_reply_msg();
		Mensaje *build_buy_msg(std::string vendedor);

		void on_client_msg(const void* msg);
		
		//Mensaje& get_mensaje();
		static int HOPCOUNT;
		
	protected:	
		void add_nodo(Mensaje *mensaje, const char*buffer);
		void less_hopcount(Mensaje *mensaje);		
		void less_nodo(Mensaje *mensaje);
		
	protected:
		Logic();
		~Logic();
		static Logic* single_instance;
};

#endif //_LOGIC_H_
