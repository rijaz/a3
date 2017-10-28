.PHONY: all clean

all: myshell

myshell: myshell.o
	gcc myshell.o -o myshell -L.local/lib -I/usr/local/include -lreadline

myshell.o: myshell.c 
	gcc myshell.c -c -o myshell.o

clean:
	-rm myshell.o myshell