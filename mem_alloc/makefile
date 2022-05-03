main : main.o memalloc.o
	g++ main.o memalloc.o -o main
memalloc.o : memalloc.cpp memalloc.h
	g++ -c memalloc.cpp -o memalloc.o
main.o : main.cpp memalloc.h
	g++ -c main.cpp -o main.o

clean:
	rm -f *.o main