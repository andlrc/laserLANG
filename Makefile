CC=gcc
CFLAGS=

PRGNAME=laserLANG
DESTDIR=
CMDDIR=$(DESTDIR)/usr/bin
MANDIR=$(DESTDIR)/usr/share/man/man1

$(PRGNAME): src/laserLANG.c
	$(CC) $(CFLAGS) -o $(PRGNAME) src/laserLANG.c

README: laserLANG.1
	MANWIDTH=80 man -l laserLANG.1 > README

install: laserLANG README laserLANG.1
	cp $(PRGNAME) $(CMDDIR)/$(PRGNAME)
	cp laserLANG.1 $(MANDIR)/$(PRGNAME).1

clean:
	rm README
	rm $(PRGNAME)

uninstall:
	rm $(CMDDIR)/$(PRGNAME)
	rm $(MANDIR)/$(PRGNAME).1
