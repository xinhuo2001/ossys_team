main : main.o FileSystem.o FCB.o
	g++ main.o FileSystem.o FCB.o -o main
main.o : main.cpp FileSystem.h
	g++ -c main.cpp -o main.o
FileSystem.o : FileSystem.cpp FCB.h
	g++ -c FileSystem.cpp -o FileSystem.o
FCB.o : FCB.cpp FCB.h
	g++ -c FCB.cpp -o FCB.o

clean:
	rm -f *.o main