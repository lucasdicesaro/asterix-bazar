## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = 

AM_CFLAGS = 

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
	keyboard.h

asterix_bazar_LDFLAGS = \
	-pthread

asterix_bazar_LDADD = \
	$(PACKAGE_LIBS) \
	../common/libasterix_bazar.a

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(PACKAGE_CFLAGS)

