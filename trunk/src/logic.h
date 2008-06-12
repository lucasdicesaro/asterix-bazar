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

		void on_client_msg(const void* msg);
		
		//Mensaje& get_mensaje();
		static int HOPCOUNT;
		
	protected:	
		void start_buy();
		void set_stock_product(std::string product_name, int cantidad);		
		
		void on_send_look_up(std::string product_name, int cantidad);
		void on_send_look_up_forward(const void* msg);		
		void on_send_replay(const void* msg);
		void on_send_replay_forward(const void* msg);
		void on_send_buy(const void* msg);
				
		void on_receive_look_up(Mensaje *mensaje);
		void on_receive_replay(Mensaje *mensaje);
		void on_receive_buy(Mensaje *mensaje);
	
		Mensaje *build_look_up_msg(std::string product_name, int cantidad);
		Mensaje *build_reply_msg(Mensaje *mensaje);
		Mensaje *build_buy_msg(Mensaje *mensaje);
		void add_nodo(Mensaje *mensaje, const char*buffer);
		void less_nodo(Mensaje *mensaje);
		void less_hopcount(Mensaje *mensaje);				
		
	protected:
		Logic();
		~Logic();
		static Logic* single_instance;
};

#endif //_LOGIC_H_
