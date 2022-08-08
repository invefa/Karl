#Makefile for karl

CC = g++

objects = main.o compiler.o token.o lexer.o basic.o bytecode.o vm.o
cpps = .\src\main.cpp .\src\compiler.cpp .\src\token.cpp .\src\lexer.cpp .\src\basic.cpp .\src\bytecode.cpp .\src\vm.cpp .\src\error.cpp .\src\string.cpp .\src\value.cpp .\src\utils.cpp

default : main.exe

# %.o : src\%.cpp
# 	$(CC) -o $*.o $< -c -I.\src

# main.exe : $(objects)
# 	$(CC) -o main.exe $(objects)

main.exe : $(cpps)
	$(CC) -o main.exe $(cpps) -I.\src

# .PHONY : clean
# clean:
# 	del *.o -rf

