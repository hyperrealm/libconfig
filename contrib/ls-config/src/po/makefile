DIRS:=$(shell find . -mindepth 1 -maxdepth 1 -name "*.po" -type f | sed -e 's/\(.*\)\..*/\1/')
PACKAGES = $(DIRS:%=%.lang)
IPACKAGES = $(DIRS:%=%.inst)
CPACKAGES = $(DIRS:%=%.cln)
RPACKAGES = $(DIRS:%=%.remov)
SHELL := /bin/bash

all: packages

packages: $(PACKAGES)

%.lang: %.po
	msgmerge -U $< ls-config.pot
	mkdir -p ../locale/$*/LC_MESSAGES
	msgfmt -c -v -o ../locale/$*/LC_MESSAGES/ls-config.mo $<

install: $(IPACKAGES)

%.inst:
	mkdir -p /usr/share/locale/$*/LC_MESSAGES
	cp -f ../locale/$*/LC_MESSAGES/ls-config.mo /usr/share/locale/$*/LC_MESSAGES

remove: $(RPACKAGES)

%.remov:
	rm -f /usr/share/locale/$*/LC_MESSAGES/ls-config.mo

clean: $(CPACKAGES) cleandir

%.cln:
	rm -Rf ../locale/$*

cleandir:
	rm -Rf ../locale