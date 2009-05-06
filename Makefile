#author:Yang Hongyang<burnef@gmail.com>

CC = gcc
#CFLAGS = "`pkg-config --cflags --libs glib-2.0 gmodule-2.0 gtk+-2.0`"

superlrc: liblrc.o mainwin.o  libaudacious.o librhythmbox.o downlrcdia.o
	$(CC) `pkg-config --cflags --libs glib-2.0 gtk+-2.0`  -o superlrc mainwin.o liblrc.o libaudacious.o librhythmbox.o downlrcdia.o

liblrc.o: liblrc.c liblrc.h libplayer.h common.h
	$(CC) `pkg-config --cflags --libs glib-2.0 gtk+-2.0` -c liblrc.c

libaudacious.o:libaudacious.c libplayer.h
	$(CC) `pkg-config --cflags --libs glib-2.0 gtk+-2.0` -c libaudacious.c

librhythmbox.o:librhythmbox.c libplayer.h
	$(CC) `pkg-config --cflags --libs glib-2.0 gtk+-2.0` -c librhythmbox.c

downlrcdia.o:downlrcdia.c downlrcdia.h
	$(CC) `pkg-config --cflags --libs glib-2.0 gtk+-2.0` -c downlrcdia.c

mainwin.o: mainwin.c common.h
	$(CC) `pkg-config --cflags --libs glib-2.0 gtk+-2.0` -c mainwin.c

