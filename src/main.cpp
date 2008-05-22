
#include "tools.h"
#include <iostream>

using namespace std;

int main()
{				
	Tools tools;
	tools.Config_Parser("lista_participante.conf");
	Tools::debug(tools.get_IP_De_Participante("LaPetisita"));
	Tools::debug(tools.get_IP_De_Participante("ElTano"));
	Tools::debug(tools.get_IP_De_Participante("ElNegro"));
}
