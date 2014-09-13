
AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(PACKAGE_CFLAGS)

lib_LIBRARIES = \
	libasterix_bazar.a

libasterix_bazar_a_SOURCES = \
	runner.cpp          \
	runner.h          \
	tools.cpp          \
	tools.h          \
	types.h          \
	socket_util.h          \
	socket_util.cpp          \
	mensaje.cpp          \
	mensaje.h          \
	nodo.h          \
	nodo.cpp          \
	logger.h          \
	logger.cpp          \
	file_config_parser.cpp          \
	file_config_parser.h          \
	logger_file_config_parser.cpp          \
	logger_file_config_parser.h          \
	file_config_reader.cpp          \
	file_config_reader.h          

#libasterix_bazar_a_LIBADD = $(PACKAGE_LIBS)

## File created by the gnome-build tools

