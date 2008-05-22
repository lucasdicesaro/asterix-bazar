//
// Class: Runner
#ifndef _RUNNER_H_
#define _RUNNER_H_

#include "types.h"
#include <queue>
#include <pthread.h>

#define ABSTRACT 0


/**
De esta clase heredan todas las entidades que corren en su propio thread.
Como la Logica, el Renderer, etc...
*/
class Runner
{
	public:
		typedef std::queue<Event> EventQueue;
		
	protected:
		typedef struct
		{
			pthread_cond_t cond;
			pthread_mutex_t mutex;
		}thread_ds;

	public:
		Runner();
		 ~Runner();
	
		/**
		Loop principal.
		*/
		void run(unsigned int timeout = INFINITE) ;
		
		/**
		Postear un evento.
		Este va sincronizado.
		*/
		void post_event(const Event& ev, bool signal = false);
		
		/**
		Wake up.
		*/
		void signal();
		
		/**
		Wait.
		*/
		int wait(unsigned int timeout = INFINITE);
		
		/**
		Locking:
		*/
		void lock();
		void unlock();
		
		/**
		Este es el handler de los eventos que deben
		overradear las classes que heredan.
		*/
		virtual void on_event(const Event&) = ABSTRACT;
		
	protected:
		/**
		Inicializa lo necesario para el threading.
		Es llamado por run antes de iterar.
		*/
		void initialize();
		
		/**
		Libera los recursos de threading.
		Es llamado por run al finalizar.
		*/
		void release();
	
	protected:
		bool running;
		bool initialized;
		EventQueue events;
		pthread_t th_self;
		thread_ds ds;
};


#endif	//_RUNNER_H_

