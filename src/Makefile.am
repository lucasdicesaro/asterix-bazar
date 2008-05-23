## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" 

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = asterix_bazar

asterix_bazar_SOURCES = \
	main.cpp       \
	runner.cpp       \
	runner.h       \
	tools.cpp       \
	tools.h       \
	types.h       

asterix_bazar_CXXFLAGS = \
	$(PACKAGE_CFLAGS)

asterix_bazar_LDFLAGS = 

asterix_bazar_LDADD = \
	$(PACKAGE_LIBS)

