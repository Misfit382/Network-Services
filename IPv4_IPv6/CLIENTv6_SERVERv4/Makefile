PROJECT = 01
SYSTEM = linux
BINDIR = .
CC = g++ -std=c++17

all: server4 client6

clean:
	@rm -rf $(BINDIR)/*.o \
	$(BINDIR)/server4
	$(BINDIR)/client6

distrib: clean
	tar -cvf $(SYSTEM)-$(PROJECT).tar $(BINDIR)/* Makefile

	
server4: server4.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

server4.o: $(BINDIR)/server4.cpp
	$(CC) $(CFLAGS) -c $(BINDIR)/$(basename $@).cpp -o $(BINDIR)/$(basename $@).o
	
client: client6.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

client.o: $(BINDIR)/client6.cpp
	$(CC) $(CFLAGS) -c $(BINDIR)/$(basename $@).cpp -o $(BINDIR)/$(basename $@).o


