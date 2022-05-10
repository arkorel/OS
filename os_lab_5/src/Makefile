all: compilation

object:
	g++ -fPIC -c realization1.cpp -o dynamic1.o
	g++ -fPIC -c realization2.cpp -o dynamic2.o

libraries: object
	g++ -shared -o libdynamic1.so dynamic1.o
	g++ -shared -o libdynamic2.so dynamic2.o

compilation: libraries
	g++ main1.cpp -L. -Wl, -rpath,. -ldynamic1 -o main1
	g++ main2.cpp -ldl -Wl, -rpath,. -o main2
