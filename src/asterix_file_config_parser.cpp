/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * asterix_file_config_parser.cpp
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

#include "asterix_file_config_parser.h"

/**
* Constructors
*/
AsterixFileConfigParser::AsterixFileConfigParser()
{
	reconnectParams = new ReconnectParamsDS();
	currentStock = new CurrentStockDS();
}

AsterixFileConfigParser::~AsterixFileConfigParser()
{
}

const char* AsterixFileConfigParser::getFileName() {
	return "lista_participante.conf";
}

void AsterixFileConfigParser::parse(char* Param, char* Value)
{
	using namespace std;

	if (strcmp(Param, "nombre_nodo") == 0)
	{
		nombre_nodo = new char[NOMBRE_NODO_SIZE];
		memset(nombre_nodo, 0, NOMBRE_NODO_SIZE);
		strcpy(nombre_nodo,  Value);
	}
	
	if (strcmp(Param, "producto_compra") == 0)
	{
		currentStock->producto_compra = Value;
	}
	if (strcmp(Param, "producto_venta") == 0)
	{
		currentStock->producto_venta = Value;
	}
		
	if  (strcmp(Param, "intentos_reconexion") == 0)
	{
		reconnectParams->intentos_reconexion = atoi(Value);
	}
	if  (strcmp(Param, "delay_reconexion") == 0)
	{
		reconnectParams->delay_reconexion = atoi(Value);
	}
	if  (strcmp(Param, "hopcount") == 0)
	{
		reconnectParams->hopcount = atoi(Value);
	}		
	
	if  (strcmp(Param, "participante") == 0) {

		char* nombre = strtok(Value, ",");
		char* ip_port = strtok(NULL , ",");						
		char* vecino1 = strtok(NULL , ",");
		char* vecino2 = strtok(NULL , ",");

		char* ip = strtok(ip_port, ":");
		int port = atoi(strtok(NULL , ":"));
		//printf ("ip: %s - port: %d\n", ip, port);
		
		// Guardo el listener_port, si el nodo que estoy parseando soy yo
		if (strcmp(nombre, nombre_nodo) == 0)
		{
			listener_port = port;
		}

		ConfigDS conf;
		strcpy(conf.nombre, nombre);
		strcpy(conf.ip, ip);			
		conf.port = port;
		strcpy(conf.vecino1, vecino1);
		strcpy(conf.vecino2, vecino2);
		listaConfig.push_back(conf);			
	}
}

std::list<ConfigDS> AsterixFileConfigParser::get_lista_config() 
{
	return listaConfig;
}

char *AsterixFileConfigParser::get_nombre_nodo() 
{
	return nombre_nodo;
}

int AsterixFileConfigParser::get_listener_port()
{
	return listener_port;
}

ReconnectParamsDS *AsterixFileConfigParser::get_reconnect_params()
{
	return reconnectParams;
}

CurrentStockDS *AsterixFileConfigParser::get_current_stock()
{
	return currentStock;
}



