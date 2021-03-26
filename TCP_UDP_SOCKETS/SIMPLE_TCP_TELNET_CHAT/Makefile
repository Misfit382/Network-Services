PROJECT = 01
SYSTEM = linux
BINDIR = .
CC = g++ -std=c++17

all: server4

clean:
	@rm -rf $(BINDIR)/*.o \
	$(BINDIR)/server4

distrib: clean
	tar -cvf $(SYSTEM)-$(PROJECT).tar $(BINDIR)/* Makefile

	
server4: server4.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

server4.o: $(BINDIR)/server4.cpp
	$(CC) $(CFLAGS) -c $(BINDIR)/$(basename $@).cpp -o $(BINDIR)/$(basename $@).o

