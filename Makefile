run: make
	./calc
make: calc.c
	gcc -o calc calc.c
