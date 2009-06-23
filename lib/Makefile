AUDACIOUS = audacious
RHYTHMBOX = rhythmbox

AUDACIOUS.c = libaudacious.c
RHYTHMBOX.c = librhythmbox.c

OBJECTS_AUDACIOUS = $(AUDACIOUS.c:%.c=%.o)
OBJECTS_RHYTHMBOX = $(RHYTHMBOX.c:%.c=%.o)

#CFLAGS += -I../include
LDLIBS += -lnsl
INSTALL_DIR=./

CFLAGS = -I../include `pkg-config --cflags --libs glib-2.0 gmodule-2.0 gtk+-2.0`

all: $(AUDACIOUS) $(RHYTHMBOX)

$(AUDACIOUS) : $(OBJECTS_AUDACIOUS)
	$(LINK.c) -shared -o $(AUDACIOUS).so $(OBJECTS_AUDACIOUS) $(CFLAGS) $(LDLIBS) $(GTKFLAGS)
	$(AR) -rcs $(AUDACIOUS).a $(OBJECTS_AUDACIOUS) $(GTKFLAGS)

$(RHYTHMBOX) : $(OBJECTS_RHYTHMBOX)
	$(LINK.c) -shared -o $(RHYTHMBOX).so $(OBJECTS_RHYTHMBOX) $(CFLAGS) $(LDLIBS) $(GTKFLAGS)
	$(AR) -rcs $(RHYTHMBOX).a $(OBJECTS_RHYTHMBOX) $(GTKFLAGS)

install:
	install -c -o bin -g bin -m 555 $(AUDACIOUS).so $(INSTALL_DIR)
	install -c -o bin -g bin -m 555 $(AUDACIOUS).a $(INSTALL_DIR)
	install -c -o bin -g bin -m 555 $(RHYTHMBOX).so $(INSTALL_DIR)
	install -c -o bin -g bin -m 555 $(RHYTHMBOX).a $(INSTALL_DIR)

clean:
	rm -rf *.o $(AUDACIOUS).so $(AUDACIOUS).a $(RHYTHMBOX).so $(RHYTHMBOX).a
