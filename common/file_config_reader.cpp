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
	FILE *CONFIG_FILE;	std::string param;
	std::string value;
	std::string::size_type n;
	std::string line;

	std::ifstream infile(fileConfigParser->getFileName());

	if (infile) {
		while (getline(infile, line)) {
			if (line[0] == DASH_CHAR) {
				// Si comienza con '#' es un comentario. Se ignora.
				continue;
			}
			n = line.find(EQUAL);
			if (n == std::string::npos) {
				// No existe '=' en esta linea
			} else {
				param = line.substr(0, n);
				value = line.substr(n+1, line.length());

				//printf ("Param: %s - Value: %s\n", Param, Value);
				fileConfigParser->parse((char *) param.c_str(), (char *) value.c_str());
			}
		}
	} else {
		sprintf(logBuffer, "No se pudo abrir archivo de configuracion %s", fileConfigParser->getFileName());
		perror(logBuffer);
		exit(-1);
	}
	infile.close();


	return 0; 
}
