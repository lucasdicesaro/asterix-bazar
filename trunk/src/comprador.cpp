
#include "comprador.h"
#include "common/tools.h"
#include <iostream>

/**
initialize
*/
void Comprador::initialize()
{
	Tools tools;
	tools.Config_Parser("lista_participante.conf");
	Tools::debug("ElTano");
	Tools::debug(tools.get_IP_De_Participante("ElTano"));
}

