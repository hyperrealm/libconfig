
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = lib

if BUILDDOC
SUBDIRS += doc
endif

if BUILDEXAMPLES
SUBDIRS += examples
endif

if BUILDTESTS
SUBDIRS += tinytest tests
endif

.PHONY: dist-rpm

dist-rpm: distcheck
	rpmbuild -ta $(distdir).tar.gz

EXTRA_DIST = \
	m4/*.m4 \
	*.sln \
	test.cfg \
	TODO \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/docs \
	debian/libconfig++9-dev.install \
	debian/libconfig++9.install \
	debian/libconfig9-dev.docs \
	debian/libconfig9-dev.install \
	debian/libconfig9.info \
	debian/libconfig9.install \
	debian/rules \
	debian/shlibs \
	debian/watch \
	libconfig.spec \
	lib/*.pc.cmake.in \
	contrib \
	CMakeLists.txt

dist-hook:
	rm -rf `find $(distdir)/contrib -name .svn`

test:
	cd tests && ./libconfig_tests
