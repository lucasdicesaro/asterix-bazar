#include "stock.h"

Stock* Stock::single_instance = NULL;

/**
* Constructors
*/
Stock::Stock()
{
	mapa_stock["sal"] = 0;
	mapa_stock["pollo"] = 0;
	mapa_stock["verdura"] = 0;
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

void Stock::decrement_stock(const std::string product_name, int cantidad)
{
	if (mapa_stock.count(product_name) > 0 && mapa_stock[product_name] > cantidad) 
		mapa_stock[product_name] = mapa_stock[product_name]-cantidad;
	//TODO: Como levantar el error si decrementa a menos de 0?
}

void Stock::increment_stock(const std::string product_name, int cantidad)
{
	if (mapa_stock.count(product_name) > 0) 
		mapa_stock[product_name] = mapa_stock[product_name] + cantidad;
}

int Stock::get_stock(const std::string product_name)
{
	if (mapa_stock.count(product_name) > 0) 
		return mapa_stock[product_name];
	else
		return -1;
}
