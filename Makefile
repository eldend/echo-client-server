all:	echo-server echo-client

echo-server:
	g++ -o echo-server echo-server.cpp

echo-client:
	g++ -o echo-client echo-client.cpp

clean:
	rm -f echo-server echo-client
