all: main.c
	gcc -g -Wall -o sl sl.c
clean: 
	$(RM) main