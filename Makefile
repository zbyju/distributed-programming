solver: folder main.o command-line-args.o
	g++ ./object/main.o ./object/command-line-args.o -o solver

run: solver
	./solver

clean:
	rm -f *.o
	rm -f *.out
	rm -f solver
	rm -rf object

folder:
	mkdir object

main.o: ./src/main.cpp
	g++ -c ./src/main.cpp -o ./object/main.o

command-line-args.o: ./src/command-line-args.cpp ./src/command-line-args.h
	g++ -c ./src/command-line-args.cpp -o ./object/command-line-args.o
