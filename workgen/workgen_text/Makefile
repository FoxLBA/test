#!/usr/bin/make -f

TOP_BUILD_DIR = ../..
include $(TOP_BUILD_DIR)/Makefile.incl

CXXFLAGS-EXTRA =

INCLUDES-EXTRA = 	-I /usr/include/mysql/ \

LIBS-EXTRA = 		-lz \
					-lssl \
					-lmysqlclient \

SOURCES = $(wildcard *.cpp)
BINARIES = $(SOURCES:.cpp=)


all: $(BINARIES)

%: %.cpp
	$(CXX) $^ $(CXXFLAGS) $(CXXFLAGS-EXTRA) $(INCLUDES) $(INCLUDES-EXTRA) $(LIBS) $(LIBS-EXTRA) -o $@

clean:
	rm -rf *.o *~ $(BINARIES)

boinc_clean:
	rm -rf download/*
