build: src/laserLANG.c
	gcc -o laserLANG src/laserLANG.c
install: build laserLANG.1
	cp ./laserLANG /usr/bin/laserLANG
	cp ./laserLANG.1 /usr/share/man/man1/laserLANG.1
clean:
	rm /usr/bin/laserLANG
	rm /usr/share/man/man1/laserLANG.1
readme: laserLANG.1
	MANWIDTH=80 man -l laserLANG.1 > README
