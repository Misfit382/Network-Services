PROJECT = 01
SYSTEM = linux
BINDIR = .
CC = g++ -std=c++17

all: server6 client4

clean:
	@rm -rf $(BINDIR)/*.o \
	$(BINDIR)/server6
	$(BINDIR)/client4

distrib: clean
	tar -cvf $(SYSTEM)-$(PROJECT).tar $(BINDIR)/* Makefile

	
server4: server6.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

server4.o: $(BINDIR)/server6.cpp
	$(CC) $(CFLAGS) -c $(BINDIR)/$(basename $@).cpp -o $(BINDIR)/$(basename $@).o
	
client: client4.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

client.o: $(BINDIR)/client4.cpp
	$(CC) $(CFLAGS) -c $(BINDIR)/$(basename $@).cpp -o $(BINDIR)/$(basename $@).o


