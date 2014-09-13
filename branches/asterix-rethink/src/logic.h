#ifndef _LOGIC_H_
#define _LOGIC_H_

#include "common/runner.h"
#include "common/mensaje.h"
#include "common/nodo.h"

class Logic : public Runner
{
	public:
		typedef std::map<std::string, EstadoPedidoID> MapaPedidos;
		
		/**
		Singleton
		*/
		static Logic* instance();
		/**
		* Implementacion requerida por Runner
		*/
		virtual void on_event(const Event& ev);

		void on_client_msg(const void* msg);
				
	protected:	
		void set_cur_prod_compra(std::string product_name);
		void set_cur_prod_venta(std::string product_name);
		void set_stock_product(std::string product_name, int cantidad);		
		
		void on_send_look_up(int cantidad);
		void on_send_look_up_forward(const void* msg);		
		void on_send_replay(const void* msg);
		void on_send_replay_forward(const void* msg);
		void on_send_buy(const void* msg);
		void on_send_buy_reply_ok(const void* msg);		
		void on_send_buy_reply_err(const void* msg);
						
		void on_receive_look_up(Mensaje *mensaje);
		void on_receive_replay(Mensaje *mensaje);
		void on_receive_buy(Mensaje *mensaje);
		void on_receive_buy_reply_ok(Mensaje *mensaje);
		void on_receive_buy_reply_err(Mensaje *mensaje);						
		
		Mensaje *build_look_up_msg(std::string product_name, int cantidad);
		Mensaje *build_reply_msg(Mensaje *mensaje);
		Mensaje *build_buy_msg(Mensaje *mensaje);
		Mensaje *build_buy_reply_ok_msg(Mensaje *mensaje);
		Mensaje *build_buy_reply_err_msg(Mensaje *mensaje);
						
		void add_nodo(Mensaje *mensaje, const char*buffer);
		void less_nodo(Mensaje *mensaje);
		void less_hopcount(Mensaje *mensaje);				
		void set_hopcount(int hopcount);
		int get_hopcount();
				
	protected:
		Logic();
		~Logic();
		static Logic* single_instance;
		int hopcount;
		MapaPedidos mapaPedidos;
};

#endif //_LOGIC_H_
