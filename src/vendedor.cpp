
#include "vendedor.h"
#include "common/tools.h"
#include <iostream>

/**
initialize
*/
void Vendedor::initialize()
{
	Tools tools;
	tools.Config_Parser("lista_participante.conf");
	Tools::debug("LaPetisita");
	Tools::debug(tools.get_IP_De_Participante("LaPetisita"));
	Tools::debug("ElNegro");
	Tools::debug(tools.get_IP_De_Participante("ElNegro"));
}

