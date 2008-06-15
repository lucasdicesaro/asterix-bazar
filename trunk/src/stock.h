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
		
		bool decrement_stock(const char* product_name, int cantidad);
		bool increment_stock(const char* product_name, int cantidad);
		
		const char* get_compro();
		void set_compro(const char* product_name);
		
		const char* get_vendo();
		void set_vendo(const char* product_name);
		
		void to_string();
		
	protected:
		Stock();
		~Stock();
		static Stock* single_instance;
		std::map<std::string, int> mapa_stock;
		std::string compro;
		std::string vendo;
};

#endif //_STOCK_H_
