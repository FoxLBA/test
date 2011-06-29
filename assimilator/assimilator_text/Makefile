#!/usr/bin/make -f

TOP_BUILD_DIR = ../..
include $(TOP_BUILD_DIR)/Makefile.incl

CXXFLAGS-EXTRA =

INCLUDES-EXTRA = 	-I /usr/include/mysql/ \

LIBS-EXTRA = 		-lz \
					-lssl \
					-lmysqlclient \

SOURCES = test_assimilator_text.cpp
BINARIES = $(SOURCES:.cpp=)


all: $(BINARIES)

sample_assimilator: assimilator.o sample_assimilator.o validate_util.o

test_assimilator_text: test_assimilator_text.o test_assimilator_handler.o validate_util.o

%.o: %.cpp
	$(CXX) -c $^ $(CXXFLAGS) $(CXXFLAGS-EXTRA) $(INCLUDES) $(INCLUDES-EXTRA) -o $@

%: %.o
	$(CXX) $^ $(CXXFLAGS) $(CXXFLAGS-EXTRA) $(INCLUDES) $(INCLUDES-EXTRA) $(LIBS) $(LIBS-EXTRA) -o $@

clean:
	rm -rf *.o *~ $(BINARIES)

boinc_clean:
	rm -rf sample_results/*
