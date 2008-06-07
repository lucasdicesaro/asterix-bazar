#include "stock.h"
#include "common/tools.h"
#include <stdlib.h>

Stock* Stock::single_instance = NULL;

/**
* Constructors
*/
Stock::Stock()
{
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
}

void Stock::increment_stock(char* product_name, int cantidad)
{
}
