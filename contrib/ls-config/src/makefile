CC=gcc
CFLAGS=-O2 -Wall
#Use for DEVELOP mode:
#DEVFLAGS=-g -D __DEVEL__
#Use for PRODUCTION mode:
DEVFLAGS=-s -fomit-frame-pointer

all: ls-config langs

ls-config: ls-config.c ls-config.pot
	$(CC) $(CFLAGS) $(DEVFLAGS) -o ../ls-config ls-config.c -L../lib -lm -lconfig 

ls-config.pot: 
	xgettext -d ls-config -o po/ls-config.pot ls-config.c

langs:
	make -C po

install:
	mkdir -p /usr/share/ls/lib
	chown root:adm /usr/share/ls/lib
	cp ../ls-config /usr/share/ls/lib/
	chown root:adm /usr/share/ls/lib
	chmod 755 /usr/share/ls/lib/ls-config
	make -C po install

clean:
	make -C po clean

remove:
	rm -f /usr/share/ls/lib/ls-config
	make -C po remove


