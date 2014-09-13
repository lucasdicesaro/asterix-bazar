/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * logger_file_config_parser.cpp
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

#include "logger_file_config_parser.h"

/**
* Constructors
*/
LoggerFileConfigParser::LoggerFileConfigParser()
{
	
}

LoggerFileConfigParser::~LoggerFileConfigParser()
{
}

const char* LoggerFileConfigParser::getFileName() {
	return "logger.properties";
}

void LoggerFileConfigParser::parse(char* Param, char* Value)
{
	if  (strcmp(Param, LOGGER_LEVEL) == 0)
	{
		if (strcmp(Value, LOGGER_DEBUG) == 0) {
			Logger::instance()->setLevel(DEBUG);
		} else if (strcmp(Value, LOGGER_INFO) == 0) {
			Logger::instance()->setLevel(INFO);
		} else if (strcmp(Value, LOGGER_WARN) == 0) {
			Logger::instance()->setLevel(WARN);
		} else if (strcmp(Value, LOGGER_ERROR) == 0) {
			Logger::instance()->setLevel(ERROR);
		}
	}
}
