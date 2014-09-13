#include "tools.h"
#include <iostream>
#include <assert.h>
#include <string.h>
#include <algorithm>  // For find
Tools* Tools::single_instance = NULL;



/**
* Constructors
*/
Tools::Tools()
{
	
}

Tools::~Tools()
{
}

Tools* Tools::instance()
{
	if (!single_instance)
		single_instance = new Tools;
	return single_instance;
}


/**
DieWithError
*/
void Tools::DieWithError(const char *errorMessage)
{
	perror(errorMessage);
	exit(0);
}

/**
duplicate
*/
char* Tools::duplicate(const std::string& str)
{
	int size = str.length()+1;
	char* buff = new char[size];
	memset(buff, 0, size);
	strcpy(buff, str.c_str());
	return buff;
}


/**
Get attribute value
*/
std::string Tools::get_attr_value(xmlpp::Element* el, const char* attr_name)
{
	using namespace xmlpp;
	assert(el);
	Attribute* attr = el->get_attribute(attr_name);
	return (attr == NULL)? "" : attr->get_value();
}




/**
Abrir log
*/
int Tools::Open_Log (char* FileName)
{
	/* abro archivo de log
	*/
	if ((LOG_FILE = fopen(FileName, "ab")) == NULL)
		return(2);
	else
		return(0);	
}
/**
Escribir log
*/
void Tools::Write_Log (char* strData)
{
	char fechahora[23];
	time_t segundos;
	struct tm *fecha;

	char buffer[2048];        


	/* obtengo fecha y hora
	*/
	segundos = time(NULL);
	fecha = localtime(&segundos);
	strftime(fechahora, 24, "%Y-%m-%d %H:%M:%S -> ", fecha);

	/* preparo string a grabar
	*/
	buffer[0] = '\0';
	strcpy(buffer, fechahora);
	strcat(buffer, strData);
	strcat(buffer, "\n\0");

	/* grabo
	*/
	fwrite(buffer, 1, strlen(buffer), LOG_FILE);
	
	return;
}
/**
Cierro log
*/
void Tools::Close_Log ()
{
	/* cierro archivo de log
	*/
	fclose(LOG_FILE);

	return;
}

