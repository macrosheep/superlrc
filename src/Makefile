#author:Yang Hongyang<burnef@gmail.com>
PROGNAME=superlrc
INSTALL_DIR=../

PLAYERLIBS=../lib/audacious.a ../lib/rhythmbox.a

CC = gcc
CFLAGS = -I../include `pkg-config --cflags --libs glib-2.0 gmodule-2.0 gtk+-2.0`

all: lrc.o downlrcdia.o mainwin.o $(PROGNAME)

$(PROGNAME): lrc.o mainwin.o downlrcdia.o
	$(CC) $(CFLAGS)  -o $(PROGNAME) mainwin.o lrc.o downlrcdia.o $(PLAYERLIBS)

lrc.o: lrc.c
	$(CC) $(CFLAGS) -c lrc.c

downlrcdia.o:downlrcdia.c
	$(CC) $(CFLAGS) -c downlrcdia.c

mainwin.o: mainwin.c
	$(CC) $(CFLAGS) -c mainwin.c

install:
	install -m 775 $(PROGNAME) $(INSTALL_DIR)
#	install -c -o bin -g bin -m 775 $(PROGNAME) $(INSTALL_DIR)

clean:
	rm -rf *.o $(PROGNAME) ../$(PROGNAME)
