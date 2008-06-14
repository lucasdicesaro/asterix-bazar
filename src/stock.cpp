#include "stock.h"
#include "common/tools.h"
#include <iostream>

using namespace std;

Stock* Stock::single_instance = NULL;

/**
* Constructors
*/
Stock::Stock()
{
	mapa_stock[PRODUCTO_SAL] = 0;
	mapa_stock[PRODUCTO_PESCADO] = 0;
	mapa_stock[PRODUCTO_VERDURA] = 10;
}

Stock::~Stock()
{
}

Stock* Stock::instance()
{
	if (!single_instance)
		single_instance = new Stock;
	return single_instance;
}

void Stock::load_stock()
{    
}

bool Stock::decrement_stock(const char* product_name, int cantidad)
{	
	bool operacion_exitosa = false;
	char logBuffer[BUFFER_SIZE];
	if (mapa_stock.count(product_name) > 0 && mapa_stock[product_name] >= cantidad) 
	{
		operacion_exitosa = true;
		mapa_stock[product_name] = mapa_stock[product_name]-cantidad;
	}
	else
	{
		if (mapa_stock.count(product_name) > 0)
		{
			sprintf(logBuffer, "Stock: decrement_stock: El producto %s no existe dentro de los productos posibles", product_name);
		}
		else
		{
			sprintf(logBuffer, "Stock: decrement_stock: No se puede decrementar el %s en %d unidades, sabiendo que tengo %d unidades", product_name, cantidad, mapa_stock[product_name]);
		}
		Tools::warn(logBuffer);
	}
	return operacion_exitosa;
}

bool Stock::increment_stock(const char* product_name, int cantidad)
{
	char logBuffer[BUFFER_SIZE];
	bool operacion_exitosa = false;
	if (mapa_stock.count(product_name) > 0) 
	{
		operacion_exitosa = true;		
		mapa_stock[product_name] = mapa_stock[product_name] + cantidad;
	}
	else
	{
		sprintf(logBuffer, "Stock: decrement_stock: El producto %s no existe dentro de los productos posibles", product_name);
		Tools::warn(logBuffer);
	}
	return operacion_exitosa;
}

int Stock::get_stock(std::string product_name)
{
	char logBuffer[BUFFER_SIZE];
	int cantidad_producto = 0;
	if (mapa_stock.count(product_name) > 0) 
		cantidad_producto = mapa_stock[product_name];
	else 
	{
		sprintf(logBuffer, "Stock: get_stock: El producto [%s] no existe en el mapa de stock!", product_name.c_str());
		Tools::warn(logBuffer);
		cantidad_producto = -1;
	}	
	return cantidad_producto;
}


void Stock::set_stock(std::string product_name, int cantidad)
{
	// Puede pisarse o acumularse, eso se decide
	mapa_stock[product_name] = cantidad;
}

char* Stock::get_compro()
{
	return compro;
}

void Stock::set_compro(char* product_name)
{
	compro = product_name;
}

char* Stock::get_vendo()
{
	return vendo;
}

void Stock::set_vendo(char* product_name)
{
	vendo = product_name;
}

void Stock::to_string()
{

	cout << "=======================================================" << endl;	
	cout << "Stock Actual: " << endl;
	cout << PRODUCTO_SAL << ": " << mapa_stock[PRODUCTO_SAL] << endl;
	cout << PRODUCTO_PESCADO << ": " << mapa_stock[PRODUCTO_PESCADO] << endl;
	cout << PRODUCTO_VERDURA << ": " << mapa_stock[PRODUCTO_VERDURA] << endl;	
	cout << "Producto de venta actual: " << vendo << endl;
	cout << "Producto de compra actual: " << compro << endl;
	cout << "=======================================================" << endl;	
}

