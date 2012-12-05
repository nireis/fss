all:
	g++ -o fss fss.cpp -Wall -O3

debug:
	g++ -o fss fss.cpp -Wall -g
