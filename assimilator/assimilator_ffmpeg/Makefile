#!/usr/bin/make -f

TOP_BUILD_DIR = ../..
include $(TOP_BUILD_DIR)/Makefile.incl

CXXFLAGS-EXTRA =

INCLUDES-EXTRA = 	-I /usr/include/mysql/ \

LIBS-EXTRA = 		-lz \
					-lssl \
					-lmysqlclient \

SOURCES = assimilator_ffmpeg.cpp
BINARIES = $(SOURCES:.cpp=)


all: $(BINARIES)

assimilator_ffmpeg: assimilator_ffmpeg.o assimilator_handler.o validate_util.o

%.o: %.cpp
	$(CXX) -c $^ $(CXXFLAGS) $(CXXFLAGS-EXTRA) $(INCLUDES) $(INCLUDES-EXTRA) -o $@

%: %.o
	$(CXX) $^ $(CXXFLAGS) $(CXXFLAGS-EXTRA) $(INCLUDES) $(INCLUDES-EXTRA) $(LIBS) $(LIBS-EXTRA) -o $@ `mysql_config --cflags --libs`

clean:
	rm -rf *.o *~ $(BINARIES)

boinc_clean:
	rm -rf sample_results/*
