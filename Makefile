build: src/laserLANG.c
	gcc -o laserLANG src/laserLANG.c
install: build laserLANG.1
	cp ./laserLANG /usr/bin/laserLANG
	cp ./laserLANG.1 /usr/share/man/man1/laserLANG.1
readme: install
	MANWIDTH=80 man 1 laserLANG > README
