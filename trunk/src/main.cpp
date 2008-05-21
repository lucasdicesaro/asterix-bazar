
#include <libxml++/libxml++.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace xmlpp;

int main()
{				
	xmlpp::Element* el;
	char* attr_name;	
	//assert(el);
	Attribute* attr = el->get_attribute(attr_name);
//	return (attr == NULL)? "" : attr->get_value();
	
//~/Projects/asterix-bazar/src$ g++ -DHAVE_CONFIG_H -I. -I.. -DPACKAGE_LOCALE_DIR=\""/usr/local//locale"\" -DPACKAGE_SRC_DIR=\""."\" -DPACKAGE_DATA_DIR=\""/usr/local/share"\" -I/usr/include/  -I/usr/include/libxml2   -g -O2 -MT asterix_bazar-main.o -MD -MP -MF .deps/asterix_bazar-main.Tpo -c -o asterix_bazar-main.o `test -f 'main.cpp' || echo './'`main.cpp
//-I/usr/include/libxml2 -I/usr/include/libxml++-2.6 -I/usr/includes/libglibmm-2.4
	
}
