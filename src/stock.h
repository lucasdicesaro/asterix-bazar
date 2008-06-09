#ifndef _STOCK_H_
#define _STOCK_H_

#include "common/tools.h"
#include "common/types.h"
#include <map>
#include <string>

class Stock
{
	public:
	
		/**
		Singleton
		*/
		static Stock* instance();
		
		void load_stock();
		int get_stock(std::string product_name);
		void decrement_stock(std::string product_name, int cantidad);
		void increment_stock(std::string product_name, int cantidad);
			
	protected:
		Stock();
		~Stock();
		static Stock* single_instance;
		std::map<std::string, int> mapa_stock;
};

#endif //_STOCK_H_
