PROJECT = 01
SYSTEM = linux
BINDIR = .
CC = g++ -std=c++17

all: server client

clean:
	@rm -rf $(BINDIR)/*.o \
	$(BINDIR)/server
	$(BINDIR)/client

distrib: clean
	tar -cvf $(SYSTEM)-$(PROJECT).tar $(BINDIR)/* Makefile

	
server4: server.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

server4.o: $(BINDIR)/server.cpp
	$(CC) $(CFLAGS) -c $(BINDIR)/$(basename $@).cpp -o $(BINDIR)/$(basename $@).o
	
client: client.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

client.o: $(BINDIR)/client.cpp
	$(CC) $(CFLAGS) -c $(BINDIR)/$(basename $@).cpp -o $(BINDIR)/$(basename $@).o


