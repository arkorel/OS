all: server client

server: server.cpp
	g++ server.cpp -lzmq -pthread -o server -w

client: client.cpp
	g++ client.cpp -lzmq -pthread -o client -w

clean:
	rm server client
