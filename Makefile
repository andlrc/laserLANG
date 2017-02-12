laserLANG: src/laserLANG.c
	gcc -o laserLANG src/laserLANG.c

README: laserLANG.1
	MANWIDTH=80 man -l laserLANG.1 > README

install: laserLANG README laserLANG.1
	cp ./laserLANG /usr/bin/laserLANG
	cp ./laserLANG.1 /usr/share/man/man1/laserLANG.1

clean:
	rm README
	rm laserLANG

uninstall:
	rm /usr/bin/laserLANG
	rm /usr/share/man/man1/laserLANG.1
