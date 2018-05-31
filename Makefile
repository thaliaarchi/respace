# https://stackoverflow.com/a/2481326/3238709
CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-Wall -g -std=c++11 #$(shell root-config --cflags)
LDFLAGS=-Wall -g -std=c++11 #$(shell root-config --ldflags)
LDLIBS=#$(shell root-config --libs)

SRCS=src/main.cpp src/parser.cpp src/reader.cpp src/vm.cpp src/writer.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

TESTSRCS=test/test.cpp
TESTOBJS=$(subst .cpp,.o,$(TESTSRCS))

all: respace

respace: $(OBJS)
	$(CXX) $(LDFLAGS) -o respace $(OBJS) $(LDLIBS)

test: $(TESTOBJS)
	$(CXX) $(LDFLAGS) -o run_tests $(TESTOBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS) $(TESTSRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS) $(TESTOBJS)

distclean: clean
	$(RM) *~ .depend

include .depend
