/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * asterix_file_config_parser.h
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

#ifndef _ASTERIX_FILE_CONFIG_PARSER_H_
#define _ASTERIX_FILE_CONFIG_PARSER_H_

#include "common/file_config_parser.h"
#include "common/types.h"
#include "common/logger.h"
#include <string.h>     /* strlen, strcmp, strtok */
#include <list>

class AsterixFileConfigParser: public FileConfigParser 
{
	public:
		typedef std::list<ConfigDS> ListaConfig;

		AsterixFileConfigParser();
		virtual ~AsterixFileConfigParser();
        virtual const char* getFileName();
		virtual void parse(char* Param, char* Value);

		std::list<ConfigDS> get_lista_config();
		ReconnectParamsDS *get_reconnect_params();
		CurrentStockDS *get_current_stock();
		char *get_nombre_nodo();
		int get_listener_port();
	protected:
		ListaConfig listaConfig;
		ReconnectParamsDS *reconnectParams;
		CurrentStockDS *currentStock;
		char *nombre_nodo;
		int listener_port;

	private:

};

#endif // _ASTERIX_FILE_CONFIG_PARSER_H_

