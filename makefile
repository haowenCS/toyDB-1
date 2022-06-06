CXX=g++
CFLAGS=-std=c++14 -O2 -Wall -g

OBJS=src/main.cc src/server/epoller.cc src/server/webserver.cc src/connect/conn.cc src/timer/timer.cc src/logger/log.cc src/logger/buffer.cc

all: server client

server:$(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o bin/server -pthread

client:src/client/client.cc
	$(CXX) $(CFLAGS) src/client/client.cc -o bin/client

clean:
	rm -rf bin/*
	
