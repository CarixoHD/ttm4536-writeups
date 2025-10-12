.SILENT:

.PHONY: all clean

all: pacman

pacman: main.c
	gcc -o pacman main.c

build: pacman
	@echo "[*] Running packer.py"
	python3 packer.py

clean:
	-rm pacman
	-rm -rf __pycache__