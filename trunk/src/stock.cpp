#include "stock.h"

Stock* Stock::single_instance = NULL;

/**
* Constructors
*/
Stock::Stock()
{
	mapa_stock["sal"] = 0;
	mapa_stock["pescado"] = 0;
	mapa_stock["verdura"] = 10;
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

void Stock::decrement_stock(char* product_name, int cantidad)
{
	char logBuffer[BUFFER_SIZE];
	if (mapa_stock.count(product_name) > 0 && mapa_stock[product_name] > cantidad) 
		mapa_stock[product_name] = mapa_stock[product_name]-cantidad;
	else
	{
		//TODO: Como levantar el error si decrementa a menos de 0?	
		sprintf(logBuffer, "Stock: decrement_stock: No se puede decrementar el %s en %d unidades, sabiendo que tengo %d unidades", product_name, cantidad, mapa_stock[product_name]);
		Tools::warn(logBuffer);
	}
}

void Stock::increment_stock(char* product_name, int cantidad)
{
	if (mapa_stock.count(product_name) > 0) 
		mapa_stock[product_name] = mapa_stock[product_name] + cantidad;
}

int Stock::get_stock(std::string product_name)
{
	if (mapa_stock.count(product_name) > 0) 
		return mapa_stock[product_name];
	else
		return -1;
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
