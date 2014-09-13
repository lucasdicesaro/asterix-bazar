## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(PACKAGE_CFLAGS)

LIBS = -lpthread $(PACKAGE_LIBS)

bin_PROGRAMS = asterix_bazar

asterix_bazar_SOURCES = \
	main.cpp       \
	router.cpp       \
	router.h       \
	listener.cpp       \
	listener.h       \
	lista_participante.conf       \
	logic.cpp       \
	logic.h       \
	stock.h       \
	stock.cpp       \
	keyboard.cpp       \
	keyboard.h       \
	logger.properties       \
	asterix_file_config_parser.cpp       \
	asterix_file_config_parser.h

asterix_bazar_LDFLAGS = \
	-lSDL

asterix_bazar_LDADD = \
	../common/libasterix_bazar.a
