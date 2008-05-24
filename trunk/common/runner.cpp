//
// Class: Runner
//
#include "runner.h"
#include "tools.h"
#include <stdexcept>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>

/**
Constructors / Destructor
*/
Runner::Runner()
{
	running = false;
	initialized = false;
}
Runner::~Runner()
{
}

/**
Runner::post_event
*/
void Runner::post_event(const Event& ev, bool signal /*= false*/)
{
	Tools::debug("Runner: post_event");
	if (!initialized)
		initialize();
	
	lock();
	events.push(ev);
	unlock();
	if (signal)
		this->signal();
}

/**
Runner::initialize
*/
void Runner::initialize()
{
	if (initialized)
		return;
	Tools::debug("Runner: initialize.");
	
	//Locking:
	pthread_cond_init(&ds.cond, NULL);
	pthread_mutex_init(&ds.mutex, NULL);
	initialized = true;

	//Post init:
	Event ev_init;
	ev_init.id = INIT;
	post_event(ev_init);
}

/**
Runner::release
*/
void Runner::release()
{
	if (!initialized)
		return;
	Tools::debug("Runner: release.");
	
	pthread_cond_destroy(&ds.cond);
	pthread_mutex_destroy(&ds.mutex);
	initialized = false;
}

/**
Locking.
*/
void Runner::lock()
{
	pthread_mutex_lock(&ds.mutex);
}
void Runner::unlock()
{
	pthread_mutex_unlock(&ds.mutex);
}

/**
Runner::run
*/
void Runner::run(unsigned int timeout)
{
	if (running)
		throw std::runtime_error("Runner: run: already running.");

	//Init:
	initialize();

	//Loop:	
	running = true;
	while (running)
	{
		//Wait:
		if (events.empty())
		{
				wait( timeout );
		}
		
		//Handle event:
		Event ev = events.front();
		switch (ev.id)
		{
			case QUIT:
				running = false;
			default:
				on_event( ev );
				break;
		}
		events.pop();
	}
	
	//Release:
	release();
}

/**
Runner::signal
*/
void Runner::signal()
{
	Tools::debug("Runner: signal");
	lock();
	pthread_cond_broadcast(&ds.cond);
	unlock();
}

/**
Runner::signal
*/
int Runner::wait(unsigned int time /*= INFINITE */)
{
	Tools::debug("Runner: wait");
	lock();
	if (time == INFINITE)
	{
		pthread_cond_wait(&ds.cond, &ds.mutex);
		unlock();
	}
	else
	{
    	struct timeval tv; 
    	gettimeofday(&tv, 0); 
    	timespec ti; 
    	ti.tv_nsec = ( tv.tv_usec + ( time % 1000 ) * 1000 ) * 1000; 
    	ti.tv_sec = tv.tv_sec + (time / 1000) + ( ti.tv_nsec / 1000000000 ); 
    	ti.tv_nsec %= 1000000000; 
		pthread_cond_timedwait(&ds.cond, &ds.mutex, &ti);
		unlock();
		Event ev;
		ev.id = RUN_TIMEOUT;
		post_event(ev, true);
	}
}
