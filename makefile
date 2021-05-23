
run: main
	./main > output.txt

main: *.cc
	g++ *.cc -o main -Wall
