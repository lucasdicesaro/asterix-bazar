#ifndef _STOCK_H_
#define _STOCK_H_

#include "common/tools.h"
#include "common/types.h"
#include <map>

class Stock
{
	public:
	
		/**
		Singleton
		*/
		static Stock* instance();
		
		void load_stock();
		
		int get_stock(std::string product_name);
		void set_stock(std::string product_name, int cantidad);
		
		void decrement_stock(char* product_name, int cantidad);
		void increment_stock(char* product_name, int cantidad);
		
		char* get_compro();
		void set_compro(char* product_name);
		
		char* get_vendo();
		void set_vendo(char* product_name);
			
	protected:
		Stock();
		~Stock();
		static Stock* single_instance;
		std::map<std::string, int> mapa_stock;
        char* compro;
        char* vendo;
};

#endif //_STOCK_H_
