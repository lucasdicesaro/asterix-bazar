/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * logger.ccp
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

#include "logger.h"



Logger* Logger::single_instance = NULL;


/**
* Constructors
*/
Logger::Logger()
{
	
}

Logger::~Logger()
{
}

Logger* Logger::instance()
{
	if (!single_instance)
		single_instance = new Logger;
	return single_instance;
	
}
int Logger::getLevel() {
	return level;
}
void Logger::setLevel(int level) {
	this->level = level;
}

/**
Debug
*/
void Logger::debug(const char* msg)
{
	if (Logger::instance()->level <= DEBUG) {
		printf("%s %s\n", LOGGER_DEBUG, msg);
	}
}

void Logger::debug(const char* nombre_nodo, const char* msg)
{
	if (Logger::instance()->level <= DEBUG) {
		printf("%s - %s %s\n", nombre_nodo, LOGGER_DEBUG, msg);
	}
}

void Logger::debug(std::string msg)
{
	if (Logger::instance()->level <= DEBUG) {
		printf("%s %s\n", LOGGER_DEBUG, msg.c_str());
	}
}

void Logger::info(std::string msg)
{
	if (Logger::instance()->level <= INFO) {
		printf("%s %s\n", LOGGER_INFO, msg.c_str());	
	}
}

void Logger::warn(std::string msg)
{
	if (Logger::instance()->level <= WARN) {
		printf("%s %s\n", LOGGER_WARN, msg.c_str());	
	}
}

void Logger::error(std::string msg)
{
	if (Logger::instance()->level <= ERROR) {
		printf("%s %s\n", LOGGER_ERROR, msg.c_str());
		//perror(msg.c_str());	
	}
}

void Logger::debug_label_value(std::string label, int value)
{
	if (Logger::instance()->level <= DEBUG) {
		std::cout << LOGGER_DEBUG << " " << label.c_str() << ": " << value << std::endl;
	}
}

void Logger::info_label_value(std::string label, int value)
{
	if (Logger::instance()->level <= INFO) {
		std::cout << LOGGER_INFO << " " << label.c_str() << ": " << value << std::endl;
	}
}


#define BUFFER_STR 128
int Logger::parseConfigFile ()
{
	FileConfigReader::parseConfigFile(new LoggerFileConfigParser());
}
