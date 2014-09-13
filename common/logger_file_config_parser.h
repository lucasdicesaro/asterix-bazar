/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * logger_file_config_parser.h
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

#ifndef _LOGGER_FILE_CONFIG_PARSER_H_
#define _LOGGER_FILE_CONFIG_PARSER_H_

#include "file_config_parser.h"
#include "types.h"
#include "logger.h"
#include <string.h>     /* strlen, strcmp, strtok */

class LoggerFileConfigParser : public FileConfigParser
{
	public:
		LoggerFileConfigParser();
		virtual ~LoggerFileConfigParser();
        virtual const char* getFileName();
		virtual void parse(char* Param, char* Value);

	protected:

	private:

};

#endif // _LOGGER_FILE_CONFIG_PARSER_H_

