#!/usr/bin/make -f

TOP_BUILD_DIR = ../..
include $(TOP_BUILD_DIR)/Makefile.incl

CXXFLAGS-EXTRA =

INCLUDES-EXTRA =

LIBS-EXTRA = 		-lz \
					-lssl \

SOURCES = assimilator_plankton.cpp
BINARIES = $(SOURCES:.cpp=)


all: $(BINARIES)

assimilator_plankton: assimilator_plankton.o assimilator_plankton_handler.o validate_util.o

%.o: %.cpp
	$(CXX) -c $^ $(CXXFLAGS) $(CXXFLAGS-EXTRA) $(INCLUDES) $(INCLUDES-EXTRA) -o $@

%: %.o
	$(CXX) $^ $(CXXFLAGS) $(CXXFLAGS-EXTRA) $(INCLUDES) $(INCLUDES-EXTRA) $(LIBS) $(LIBS-EXTRA) -o $@

clean:
	rm -rf *.o *~ $(BINARIES)

boinc_clean:
	rm -rf sample_results/*
