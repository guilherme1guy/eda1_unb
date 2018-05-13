clean:
	rm -r -f build/*
	rmdir build
	
build:
	mkdir build/
	gcc src/main.c -o build/main -lm -Wall

copy:
	cp references/contatos.txt build/contatos.txt

run:
	(cd build; ./main 1)

all: clean build copy run

all-no-copy: clean build run