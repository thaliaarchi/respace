# https://stackoverflow.com/a/2481326/3238709
CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-Wall -g -std=c++11 #$(shell root-config --cflags)
LDFLAGS=-Wall -g -std=c++11 #$(shell root-config --ldflags)
LDLIBS=#$(shell root-config --libs)

SRCS=src/main.cpp src/parser.cpp src/reader.cpp src/vm.cpp src/writer.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: wspace

wspace: $(OBJS)
	$(CXX) $(LDFLAGS) -o wspace $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend
