CC	= gcc
CFLAGS	= -O3 -std=c99 -Wall -Werror -Wextra -Wmissing-prototypes \
	  -Wstrict-prototypes -Wold-style-definition

PRGNAME	= laserLANG
CMDDIR	= $(DESTDIR)/usr/bin
MANDIR	= $(DESTDIR)/usr/share/man/man1

# Build

$(PRGNAME):	src/laserLANG.c
	$(CC) $(CFLAGS) -o $(PRGNAME) src/laserLANG.c

# Install

install:	laserLANG README laserLANG.1
	cp $(PRGNAME) $(CMDDIR)/$(PRGNAME)
	cp laserLANG.1 $(MANDIR)/$(PRGNAME).1

uninstall:
	rm $(CMDDIR)/$(PRGNAME)
	rm $(MANDIR)/$(PRGNAME).1

# Docs

README:	laserLANG.1
	MANWIDTH=80 man -l laserLANG.1 > README

# Utilities

clean:
	-rm $(PRGNAME)
