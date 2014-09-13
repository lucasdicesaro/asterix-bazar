/*
 * asterix-config-data.cpp
 *
 *  Created on: Sep 13, 2014
 *      Author: lucas
 */

#include "asterix_config_data.h"

AsterixConfigData* AsterixConfigData::single_instance = NULL;

AsterixConfigData::AsterixConfigData() {
}

AsterixConfigData::~AsterixConfigData() {
}

AsterixConfigData* AsterixConfigData::instance()
{
	if (!single_instance)
		single_instance = new AsterixConfigData;
	return single_instance;

}

void AsterixConfigData::parseConfigFile ()
{
	AsterixFileConfigParser *asterixFileConfigParser = new AsterixFileConfigParser();
	FileConfigReader::parseConfigFile(asterixFileConfigParser);

	set_lista_config(asterixFileConfigParser->get_lista_config());
	set_reconnect_params(asterixFileConfigParser->get_reconnect_params());
	set_current_stock(asterixFileConfigParser->get_current_stock());
	set_nombre_nodo(asterixFileConfigParser->get_nombre_nodo());
	set_listener_port(asterixFileConfigParser->get_listener_port());
}

std::list<ConfigDS> AsterixConfigData::get_lista_config()
{
	return listaConfig;
}
void AsterixConfigData::set_lista_config(std::list<ConfigDS> listaConfig)
{
	this->listaConfig = listaConfig;
}

ReconnectParamsDS *AsterixConfigData::get_reconnect_params()
{
	return reconnectParams;
}
void AsterixConfigData::set_reconnect_params(ReconnectParamsDS *reconnectParams)
{
	this->reconnectParams = reconnectParams;
}

CurrentStockDS *AsterixConfigData::get_current_stock()
{
	return currentStock;
}

void AsterixConfigData::set_current_stock(CurrentStockDS *currentStock)
{
	this->currentStock = currentStock;
}

char *AsterixConfigData::get_nombre_nodo()
{
	return nombre_nodo;
}

void AsterixConfigData::set_nombre_nodo(char *nombre_nodo)
{
	this->nombre_nodo = nombre_nodo;
}

int AsterixConfigData::get_listener_port()
{
	return listener_port;
}

void AsterixConfigData::set_listener_port(int listener_port)
{
	this->listener_port = listener_port;
}


char *AsterixConfigData::get_IP_De_Participante(const char *nombre) {

	char *key = Tools::duplicate(nombre);
	ConfigDS* elementoConfig = NULL;
	for (ListaConfig::iterator it = listaConfig.begin(); it != listaConfig.end() && elementoConfig == NULL; it++)
	{
		ConfigDS tmp = *it;
		if (strcmp(tmp.nombre, key) == 0) {
			elementoConfig = &tmp;
		}
	}
	return Tools::duplicate (elementoConfig->ip);
}

ConfigDS *AsterixConfigData::get_info_nodo(const char *nombre) {

	char *key = Tools::duplicate(nombre);

	ConfigDS* elementoConfig = NULL;
	for (ListaConfig::iterator it = listaConfig.begin(); it != listaConfig.end() && elementoConfig == NULL; it++)
	{
		ConfigDS tmp = *it;
		if (strcmp(tmp.nombre, key) == 0) {
			elementoConfig = new ConfigDS();
			strcpy(elementoConfig->nombre, tmp.nombre);
			strcpy(elementoConfig->ip, tmp.ip);
			elementoConfig->port = tmp.port;
			strcpy(elementoConfig->vecino1, tmp.vecino1);
			strcpy(elementoConfig->vecino2, tmp.vecino2);
		}
	}

	return elementoConfig;
}
