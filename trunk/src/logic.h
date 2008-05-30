#ifndef _LOGIC_H_
#define _LOGIC_H_

#include "common/runner.h"

class Logic : public Runner
{
	public:
		
		/**
		Singleton
		*/
		static Logic* instance();
		/**
		* Implementacion requerida por Runner
		*/
		virtual void on_event(const Event& ev);

		std::string build_msg();
		
		void on_client_msg(const void* msg);
	protected:	

		
	
	protected:
		Logic();
		~Logic();
		static Logic* single_instance;				
		
};

#endif //_LOGIC_H_
