CFLAGS= -g -o0 -Wall
all: list.o main.o commands.o aux-fun.o p2 clean
	@echo Finalizado
main.o: main.c
	@gcc -c main.c
	
list.o: list.c
	@gcc -c list.c
	
commands.o: commands.c
	@gcc -c commands.c
aux-fun.o: aux-fun.c
	@gcc -c aux-fun.c
p2:	main.o list.o commands.o aux-fun.o
	@gcc -Wall -o p3 main.o list.o commands.o aux-fun.o
	@echo Compilado correcto
clean:
	@rm *.o
	@echo Limpieza correcta
