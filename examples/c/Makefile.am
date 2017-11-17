
check_PROGRAMS = example1 example2 example3 example4
noinst_PROGRAMS=$(check_PROGRAMS)

TESTS = $(check_PROGRAMS)

AM_CPPFLAGS = -I $(top_srcdir)/lib

LDADD = $(top_builddir)/lib/libconfig.la

EXTRA_DIST = \
	example.cfg \
	example4.cfg \
	cfg_includes/*.cfg \
	cfg_includes/cfg_subincludes/*.cfg \
	*.vcproj \
	*.vcxproj
