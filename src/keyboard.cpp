#include "keyboard.h"
#include "logic.h"
#include "router.h"
#include <iostream>
#include <signal.h>
#include <string.h>

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
		  	elegir_accion();
		  break;			

		default:
			Tools::debug("Keyboard: on event: *UNKNOWN*.");
			break;
	}
}

void Keyboard::elegir_accion()
{
	int tecla;		
	do
    {
		cout<<"MENU: " << endl;
		cout<<"1.Llenar stock " << endl;
		cout<<"2.Elegir producto de compra" << endl;
		cout<<"3.Elegir producto de venta" << endl;
		cout<<"4.Lanzar compra " << endl;
		cout<<"5.Mostrar Stock " << endl;
		cout<<"6.Intentar reconectarse con vecinos " << endl;				
		cout<<"8.Salir" << endl;
		cout<<" Haga su eleccion:  " << endl;
		
		tecla = obtener_tecla();
		
		if (!en_operacion)
		{
			switch(tecla)
			{
				case 1:
					Tools::debug("Keyboard: elegir_accion: Tecla 1");
					procesar_add_stock();
					break;
				case 2:
					Tools::debug("Keyboard: elegir_accion: Tecla 2");			
					procesar_set_producto_compra();
					break;
				case 3:
					Tools::debug("Keyboard: elegir_accion: Tecla 3");			
					procesar_set_producto_venta();
					break;
				case 4:
					Tools::debug("Keyboard: elegir_accion: Tecla 4");			
					procesar_lanzar();
					break;
				case 5:
					Tools::debug("Keyboard: elegir_accion: Tecla 5");			
					showstock();
					break;					
				case 6:
					Tools::debug("Keyboard: elegir_accion: Tecla 6");			
					try_reconnect();
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

void Keyboard::procesar_add_stock()
{
	Tools::debug("Keyboard: procesar_add_stock:");	
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
			Tools::debug("Keyboard: procesar_add_stock: Tecla 1");
			ev.id = SET_STOCK_SAL; 
			product_name = PRODUCTO_SAL;
			break;

		case 2:
			Tools::debug("Keyboard: procesar_add_stock: Tecla 2");
			ev.id = SET_STOCK_PESCADO; 
			product_name = PRODUCTO_PESCADO;
			break;
			
		case 3:
			Tools::debug("Keyboard: procesar_add_stock: Tecla 3");
			ev.id = SET_STOCK_VERDURA; 
			product_name = PRODUCTO_VERDURA;
			break;
	}	
	
	sprintf(logBuffer, "Ingrese la cantidad de [%s] que quiere setear en el stock", product_name.c_str());
	strcpy(buffer, obtener_cadena(logBuffer));
	if (en_operacion)
	{
		Tools::warn("El nodo esta en operacion. No se admite ingreso de teclado. Volviendo el menu");	
		return; // Si el nodo entro en operacion, fuerzo a la salida del metodo
	}
	
	sprintf(logBuffer, "Keyboard: procesar_add_stock: Se apreto [%s]", buffer);	
   	Tools::debug(logBuffer);
	ev.tag = Tools::instance ()->duplicate(buffer);
	Logic::instance()->post_event(ev, true);
}




void Keyboard::procesar_set_producto_compra()
{
	Tools::debug("Keyboard: procesar_set_producto_compra:");	
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
			Tools::debug("Keyboard: procesar_set_producto_compra: Tecla 1");
			ev.id = SET_CUR_PROD_COMPRA; 
			product_name = PRODUCTO_SAL;
			break;
		case 2:
			Tools::debug("Keyboard: procesar_set_producto_compra: Tecla 2");
			ev.id = SET_CUR_PROD_COMPRA; 
			product_name = PRODUCTO_PESCADO;
			break;
		case 3:
			Tools::debug("Keyboard: procesar_set_producto_compra: Tecla 3");
			ev.id = SET_CUR_PROD_COMPRA; 
			product_name = PRODUCTO_VERDURA;
			break;
	}
	
	sprintf(logBuffer, "Keyboard: procesar_set_producto_compra: Se eligio [%s]", product_name.c_str());	
   	Tools::debug(logBuffer);
	ev.tag = Tools::instance ()->duplicate(product_name);
	Logic::instance()->post_event(ev, true);
	
}

void Keyboard::procesar_set_producto_venta()
{
	Tools::debug("Keyboard: procesar_set_producto_venta:");	
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
			Tools::debug("Keyboard: procesar_set_producto_venta: Tecla 1");
			ev.id = SET_CUR_PROD_VENTA; 
			product_name = PRODUCTO_SAL;
			break;
		case 2:
			Tools::debug("Keyboard: procesar_set_producto_venta: Tecla 2");
			ev.id = SET_CUR_PROD_VENTA; 
			product_name = PRODUCTO_PESCADO;
			break;
		case 3:
			Tools::debug("Keyboard: procesar_set_producto_venta: Tecla 3");
			ev.id = SET_CUR_PROD_VENTA; 
			product_name = PRODUCTO_VERDURA;
			break;
	}
	
	sprintf(logBuffer, "Keyboard: procesar_set_producto_venta: Se eligio [%s]", product_name.c_str());	
   	Tools::debug(logBuffer);
	ev.tag = Tools::instance ()->duplicate(product_name);
	Logic::instance()->post_event(ev, true);	
}

void Keyboard::procesar_lanzar()
{
	char *logBuffer = new char[BUFFER_SIZE];		
	char *buffer = new char[BUFFER_SIZE];	
	memset(buffer, 0, BUFFER_SIZE);	
	
	strcpy(buffer, obtener_cadena("Ingrese la cantidad de unidades que quiere comprar:"));

	Event ev;	
	sprintf(logBuffer, "Keyboard: procesar_lanzar: Se apreto [%s]", buffer);	
   	Tools::debug(logBuffer);
	ev.id = DO_LOOKUP;	
	ev.tag = Tools::instance ()->duplicate(buffer);		
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
		
		tecla = obtener_tecla();
		
		if (en_operacion)
		{
			Tools::warn("El nodo esta en operacion. No se admite ingreso de teclado. Volviendo el menu");	
			return -1;
		}

	} while (tecla != 1 && tecla != 2 && tecla != 3);

	return tecla;
}


char *Keyboard::obtener_cadena(const char*mensaje_prompt)
{
	char *buffer = new char[BUFFER_SIZE];	
	memset(buffer, 0, BUFFER_SIZE);	
	
	// Limpio el buffer de teclado
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF);	
	
	cout << mensaje_prompt << endl;
	
	fgets(buffer, BUFFER_SIZE, stdin);	
}

int Keyboard::obtener_tecla()
{
	int tecla;
	// Limpio el buffer de teclado
	//int ch;
	//while ((ch = getchar()) != '\n' && ch != EOF);		
	//fflush(stdin);
	cin >> tecla;
	return tecla;
}

void Keyboard::showstock()
{
	Event ev;
	ev.id = KB_SHOW_STOCK;
	Logic::instance()->post_event(ev, true);
}

void Keyboard::try_reconnect()
{
	Event ev;
	ev.id = INIT;
	Router::instance()->post_event(ev, true);
}
