solver: main.o command-line-args.o
	g++ main.o command-line-args.o -o solver

run: solver
	./solver

clean:
	rm *.o solver

main.o: ./src/main.cpp
	g++ -c ./src/main.cpp

command-line-args.o: ./src/command-line-args.cpp ./src/command-line-args.h
	g++ -c ./src/command-line-args.cpp
