clean:
	rm -r -f build/*
	rmdir build
	
build:
	mkdir build/
	gcc src/main.c -o build/main -lm -Wall

run:
	./build/main $(CURDIR)/DataSet 1

all: clean build run