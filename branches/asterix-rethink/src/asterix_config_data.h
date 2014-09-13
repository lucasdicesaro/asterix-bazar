/*
 * asterix-config-data.h
 *
 *  Created on: Sep 13, 2014
 *      Author: lucas
 */

#ifndef ASTERIX_CONFIG_DATA_H_
#define ASTERIX_CONFIG_DATA_H_

#include "common/types.h"
#include "common/tools.h"
#include "common/file_config_reader.h"
#include "asterix_file_config_parser.h"

class AsterixConfigData {
public:
	typedef std::list<ConfigDS> ListaConfig;
	/**
	 * Singleton
	 */
	static AsterixConfigData* instance();

	void parseConfigFile ();

	std::list<ConfigDS> get_lista_config();
	void set_lista_config(std::list<ConfigDS> listaConfig);
	ReconnectParamsDS *get_reconnect_params();
	void set_reconnect_params(ReconnectParamsDS *reconnectParamsDS);
	CurrentStockDS *get_current_stock();
	void set_current_stock(CurrentStockDS *currentStockDS);

	/** Obtengo un participante */
	char *get_IP_De_Participante(const char *nombre);
	ConfigDS *get_info_nodo(const char *nombre);
	char *get_nombre_nodo();
	void set_nombre_nodo(char *nombre_nodo);
	int get_listener_port();
	void set_listener_port(int listener_port);

protected:
	AsterixConfigData();
	virtual ~AsterixConfigData();
	static AsterixConfigData* single_instance;


private:
	ListaConfig listaConfig;
	ReconnectParamsDS *reconnectParams;
	CurrentStockDS *currentStock;
	char *nombre_nodo;
	int listener_port;
};

#endif /* ASTERIX_CONFIG_DATA_H_ */
