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
		void elegir_accion();
		
		void procesar_add_stock();
		void procesar_set_producto_compra();		
		void procesar_set_producto_venta();		
		void procesar_lanzar();		
		void showstock();
		void try_reconnect();

		int elegir_producto();
				
		char *obtener_cadena(const char*mensaje_prompt);
		int obtener_tecla();

	protected:	
	
		Keyboard();
		~Keyboard();
		static Keyboard* single_instance;				
};

#endif //_KEYBOARD_H_
