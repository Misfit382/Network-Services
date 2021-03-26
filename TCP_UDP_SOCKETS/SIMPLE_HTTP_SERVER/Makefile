PROJECT = 01
SYSTEM = linux
SRCDIR = src
BINDIR = bin
LIBDIR = lib
INCDIR = include
CC = g++ -std=c++17


all: main.o filehandler.o request_builder.o server.o server 

clean:
	@rm -rf $(BINDIR)/*.o \
	$(LIBDIR)/*
	
server: $ main.o filehandler.o request_builder.o server.o
	$(CC) $(CFLAGS) $(LIBDIR)/server.o $(LIBDIR)/request_builder.o $(LIBDIR)/filehandler.o $(LIBDIR)/main.o -o $(BINDIR)/$@ -pthread
	

	
distrib: clean
	tar -cvf $(SYSTEM)-$(PROJECT).tar $(SRCDIR)/* Makefile

main.o: $(SRCDIR)/main.cpp 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).cpp -o $(LIBDIR)/$(basename $@).o

filehandler.o: $(SRCDIR)/filehandler.cpp 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).cpp -o $(LIBDIR)/$(basename $@).o
	
request_builder.o: $(SRCDIR)/request_builder.cpp 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).cpp -o $(LIBDIR)/$(basename $@).o

server.o: $(SRCDIR)/server.cpp 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).cpp -o $(LIBDIR)/$(basename $@).o
	

