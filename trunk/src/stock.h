#ifndef _STOCK_H_
#define _STOCK_H_

class Stock
{
	public:
		
		/**
		Singleton
		*/
		static Stock* instance();
		
		void load_stock();
		void decrement_stock(char* product_name, int cantidad);
		void increment_stock(char* product_name, int cantidad);
			
	protected:
		Stock();
		~Stock();
		static Stock* single_instance;
};

#endif //_STOCK_H_
