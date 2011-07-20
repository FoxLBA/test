#!/usr/bin/make -f

include Makefile.incl

TARGET_DAEMONS = assimilator/assimilator workgen/workgen
TARGET_APPS = apps/test
TARGET_VERSIONS = text ffmpeg plankton

all:
	for d in $(TARGET_DAEMONS); do for v in $(TARGET_VERSIONS); do make -C "$$d"_"$$v"; done; done
	for d in $(TARGET_APPS); do for v in $(TARGET_VERSIONS); do make -C "$$d"-"$$v"; done; done

clean:
	for d in $(TARGET_DAEMONS); do for v in $(TARGET_VERSIONS); do make -C "$$d"_"$$v" clean; done; done
	for d in $(TARGET_APPS); do for v in $(TARGET_VERSIONS); do make -C "$$d"-"$$v" clean; done; done

remote:
	rsync -rchh --delete-after --progress . $(SERVER_PATH):build/boinc/
	ssh $(SERVER_PATH) "make -C build/boinc"
