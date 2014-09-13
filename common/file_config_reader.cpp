/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * file_config_reader.cpp
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

#include "file_config_reader.h"



#define BUFFER_STR 128
int FileConfigReader::parseConfigFile (FileConfigParser *fileConfigParser)
{
	using namespace std;

	char logBuffer[BUFFER_SIZE];
	FILE *CONFIG_FILE;	char Line[BUFFER_STR];
	char* Param;
	char* Value;

	/* abro archivo 
	*/
	if ((CONFIG_FILE = fopen(fileConfigParser->getFileName(), "r")) == NULL) {
		sprintf(logBuffer, "No se pudo abrir archivo de configuracion %s", fileConfigParser->getFileName());
		perror(logBuffer);
		exit(-1);
	}

	/* Leo la primera linea 
    */
	if (fgets(Line, sizeof(Line),  CONFIG_FILE) == NULL) {
		/* Ignoro el Null, si es fin de archivo se controla con feof */
	}

	/* Reemplazo el '\n' al final del registro por '\0'
    */
	Line[strlen(Line)-1] = '\0';


	/* Valido que el archivo  no este vacio
	*/
	if (feof(CONFIG_FILE)) {
		sprintf(logBuffer, "El archivo de configuracion %s esta vacio", fileConfigParser->getFileName());
		perror(logBuffer);
		exit(-1);
	}

	/* Recorro y parseo los registros
    */
	while (!feof(CONFIG_FILE))
	{
		const char initChar = Line[0];
		if (strcmp(&initChar, DASH) == 0) {
			// Si comienza con '#' es un comentario. Se ignora.
			continue;
		}

		Param = strtok(Line, "=");
		//printf ("Line: %s\n", Line);
		Value  = strtok(NULL , "=");

		//printf ("Param: %s - Value: %s\n", Param, Value);
		fileConfigParser->parse(Param, Value);

		/*	Leo la siguiente linea 
    	*/
		if (fgets(Line, sizeof(Line), CONFIG_FILE) == NULL) {
			/* Ignoro el Null, si es fin de archivo se controla con feof */
		}

		Line[strlen(Line)-1] = '\0';
	}

	/* Cierro el archivo 
	*/
	fclose(CONFIG_FILE);	
	
	return 0; 
}
