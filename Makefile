# Makefile for ISA project FIT VUT
# Fridolin Pokorny
# fridex.devel@gmail.com

CC = g++
#CXXFLAGS = -Wall -std=c++98 -O0 -ggdb -Wall
CXXFLAGS = -Wall -std=c++98 -O2 -fomit-frame-pointer
LDFLAGS =

SRCS = tcpsearch.cpp arg.cpp host.cpp connect.cpp
HDRS = arg.h tcpsearch.h connect.h host.h arg-inl.h
OBJS = tcpsearch.o arg.o host.o connect.o
AUX  = Makefile README
DOC  = manual.pdf
PKG  = project.tar

.PHONY: clean doc pack

all: tcpsearch

tcpsearch: $(OBJS)
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $@

clean:
	rm -f $(PKG) $(OBJS)

doc:
	cd DOC && make

pack: doc
	mv DOC/$(DOC) .
	make -C DOC/ clean
	tar -cf $(PKG) $(SRCS) $(HDRS) $(AUX) $(DOC) DOC/

