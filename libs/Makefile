#!/usr/bin/make -f

TOP_BUILD_DIR = ..
include $(TOP_BUILD_DIR)/Makefile.incl

CXXFLAGS-EXTRA = -c

INCLUDES-EXTRA = 	-I /usr/include/mysql/ \

LIBS-EXTRA =

SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
OBJFILES = $(SOURCES:.cpp=.o)
STATIC_LIBS = libplankton.a


all: $(STATIC_LIBS) $(HEADERS)
	cp -t $(TOP_BUILD_DIR)/deps/libs/$(ARCH)/ $(STATIC_LIBS)
	cp -t $(TOP_BUILD_DIR)/deps/headers/ $(HEADERS)

libplankton.a: $(OBJFILES)
	ar rc $@ $(OBJFILES)

%.o: %.cpp
	$(CXX) $^ $(CXXFLAGS) $(CXXFLAGS-EXTRA) $(INCLUDES) $(INCLUDES-EXTRA) $(LIBS) $(LIBS-EXTRA) -o $@

clean:
	rm -rf *.o *.a *~
