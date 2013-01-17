#!/usr/bin/make -f

include Makefile.incl

TARGET_DAEMONS = assimilator workgen
TARGET_APPS = apps/test
#TARGET_VERSIONS = text ffmpeg plankton
TARGET_VERSIONS = plankton

PROJECT_DIR = ~/projects/plankton

all:
	$(MAKE) -C libs
	for d in $(TARGET_DAEMONS); do for v in $(TARGET_VERSIONS); do $(MAKE) -C "$$d"/"$$d"_"$$v"; done; done
	for d in $(TARGET_APPS); do for v in $(TARGET_VERSIONS); do $(MAKE) -C "$$d"-"$$v"; done; done


clean:
	$(MAKE) -C libs clean
	for d in $(TARGET_DAEMONS); do for v in $(TARGET_VERSIONS); do $(MAKE) -C "$$d"/"$$d"_"$$v" clean; done; done
	for d in $(TARGET_APPS); do for v in $(TARGET_VERSIONS); do $(MAKE) -C "$$d"-"$$v" clean; done; done

install:
	for d in $(TARGET_DAEMONS); do for v in $(TARGET_VERSIONS); do cp "$$d"/"$$d"_"$$v"/"$$d"_"$$v" $(PROJECT_DIR)/bin/; done; done
	#for d in $(TARGET_APPS); do for v in $(TARGET_VERSIONS); do cp "$$d"/"$$d"_"$$v"/"$$d"_"$$v" $(PROJECT_DIR)/apps/; done; done
