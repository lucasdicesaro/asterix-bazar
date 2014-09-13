/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * logger.h
 * Copyright (C) 2014 Lucas <lucasdicesaro@gmail.com>
 *
 * asterix is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * asterix is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "types.h"
#include "file_config_reader.h"
#include "file_config_parser.h"
#include "logger_file_config_parser.h"

#include <stdio.h>      /* printf, fgets */
#include <string.h>     /* strlen, strcmp, strtok */
#include <stdlib.h>     /* exit, EXIT_FAILURE */

#define LOGGER_LEVEL "level"

#define LOGGER_DEBUG "DEBUG"
#define LOGGER_INFO "INFO"
#define LOGGER_WARN "WARN"
#define LOGGER_ERROR "ERROR"

typedef enum
{
	DEBUG=0,
	INFO,
	WARN,
	ERROR
} LoggerLevel;

class Logger
{
	public:
		/**
		 * Singleton
		 */
		static Logger* instance();

		/**
		 *	
		 */
		void static debug(const char* msg);
		void static debug(const char* nombre_nodo, const char* msg);
		void static debug(std::string msg);
		void static info(std::string msg);
		void static warn(std::string msg);
		void static error(std::string msg);		
		void static debug_label_value(std::string label, int value);
		void static info_label_value(std::string label, int value);

		int getLevel();
		void setLevel(int level);
		int parseConfigFile ();

	protected:
		Logger();
		~Logger();
		static Logger* single_instance;

	private:
		int level;
};

#endif // _LOGGER_H_

