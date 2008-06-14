#include "keyboard.h"
#include "logic.h"
#include <iostream>
#include <signal.h>

using namespace std;

Keyboard* Keyboard::single_instance = NULL;

extern bool en_operacion;

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
	switch (ev.id)
	{
		case INIT:
			Tools::debug("Keyboard: on_event: INIT:");
		  break;
			
		case KB_SHOW_MENU:
			Tools::debug("Keyboard: on_event: KB_SHOW_MENU:");
		  	get_key();
		  break;			

		default:
			Tools::debug("Keyboard: on event: *UNKNOWN*.");
			break;
	}
}

void Keyboard::get_key()
{
	int tecla;		
	do
    {
		cout<<"MENU: " << endl;
		cout<<"1.Llenar stock " << endl;
		cout<<"2.submenu2 " << endl;
		cout<<"3.submenu2 " << endl;
		cout<<"4.Lanzar compra " << endl;
		cout<<"8.Salir" << endl;
		cout<<" Haga su eleccion:  " << endl;
		cin>>tecla;
		
		if (!en_operacion)
		{
			switch(tecla)
			{
				case 1:
					Tools::debug("Keyboard: get_key: Tecla 1");
					procesar_submenu1();
					break;
				//case 2:delrecord();break;
				//case 3:modrecord();break;
				case 4:
					Tools::debug("Keyboard: get_key: Tecla 4");			
					procesar_lanzar();
					break;
			}
		}
		else
		{
			Tools::warn("El nodo esta en operacion. No se admite ingreso de teclado");	
		}
	} while (tecla != 8);

	// Fuerzo la salida
	raise(SIGINT);
}

void Keyboard::procesar_submenu1()
{
	Tools::debug("Keyboard: procesar_submenu1:");	
	char *logBuffer = new char[BUFFER_SIZE];		
	char *buffer = new char[BUFFER_SIZE];	
	memset(buffer, 0, BUFFER_SIZE);
	
	int tecla = elegir_producto();	
	
	if (en_operacion)
	{
		Tools::warn("El nodo esta en operacion. No se admite ingreso de teclado. Volviendo el menu");	
		return; // Si el nodo entro en operacion, fuerzo a la salida del metodo
	}
	
	Event ev;
	std::string product_name;
	switch(tecla)
	{
		case 1:
			Tools::debug("Keyboard: get_key: Tecla 1");
			ev.id = SET_STOCK_SAL; 
			product_name = PRODUCTO_SAL;
			break;

		case 2:
			Tools::debug("Keyboard: get_key: Tecla 2");
			ev.id = SET_STOCK_PESCADO; 
			product_name = PRODUCTO_PESCADO;
			break;
			
		case 3:
			Tools::debug("Keyboard: get_key: Tecla 3");
			ev.id = SET_STOCK_VERDURA; 
			product_name = PRODUCTO_VERDURA;
			break;
	}	
	
	// Limpio el buffer de teclado
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF);

	
	cout<<"Ingrese la cantidad de [" << product_name << "] que quiere setear en el stock" << endl;
	fgets(buffer, BUFFER_SIZE, stdin);
	if (en_operacion)
	{
		Tools::warn("El nodo esta en operacion. No se admite ingreso de teclado. Volviendo el menu");	
		return; // Si el nodo entro en operacion, fuerzo a la salida del metodo
	}
	
	sprintf(logBuffer, "Se apreto [%s]", buffer);	
   	Tools::debug(logBuffer);
	ev.tag = Tools::instance ()->duplicate(buffer);
	Logic::instance()->post_event(ev, true);
}

void Keyboard::procesar_lanzar()
{
	Event ev;
	ev.id = DO_LOOKUP;
	Logic::instance()->post_event(ev, true);
}

int Keyboard::elegir_producto()
{
	int tecla;
	do
    {
		cout<<"MENU: " << endl;
		cout<<"1." << PRODUCTO_SAL << endl;
		cout<<"2." << PRODUCTO_PESCADO << endl;
		cout<<"3." << PRODUCTO_VERDURA << endl;
		cout<<" Haga su eleccion:  " << endl;
		cin>>tecla;
		if (en_operacion)
		{
			Tools::warn("El nodo esta en operacion. No se admite ingreso de teclado. Volviendo el menu");	
			return -1;
		}

	} while (tecla != 1 && tecla != 2 && tecla != 3);

	return tecla;
}


