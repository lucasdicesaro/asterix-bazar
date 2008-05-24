## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = \
	common \
	src 

asterix_bazardocdir = ${prefix}/doc/asterix_bazar
asterix_bazardoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

EXTRA_DIST = $(asterix_bazardoc_DATA)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

