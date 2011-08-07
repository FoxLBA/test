#!/usr/bin/make -f

include Makefile.incl

TARGET_DAEMONS = assimilator workgen
TARGET_APPS = apps/test
TARGET_VERSIONS = text ffmpeg plankton

all:
	$(MAKE) -C libs
	for d in $(TARGET_DAEMONS); do for v in $(TARGET_VERSIONS); do $(MAKE) -C "$$d"/"$$d"_"$$v"; done; done
	for d in $(TARGET_APPS); do for v in $(TARGET_VERSIONS); do $(MAKE) -C "$$d"-"$$v"; done; done


clean:
	$(MAKE) -C libs clean
	for d in $(TARGET_DAEMONS); do for v in $(TARGET_VERSIONS); do $(MAKE) -C "$$d"/"$$d"_"$$v" clean; done; done
	for d in $(TARGET_APPS); do for v in $(TARGET_VERSIONS); do $(MAKE) -C "$$d"-"$$v" clean; done; done

remote:
	rsync -rchh --delete-after --progress . $(SERVER_PATH):$(SERVER_REPO)/
	ssh $(SERVER_PATH) "make -C build/boinc all rinstall"

rinstall:
	for d in $(TARGET_DAEMONS); do for v in $(TARGET_VERSIONS); do cp "$$d"/"$$d"_"$$v"/"$$d"_"$$v" ~/projects/test/bin/; done; done
	#for d in $(TARGET_APPS); do for v in $(TARGET_VERSIONS); do cp "$$d"/"$$d"_"$$v"/"$$d"_"$$v" ~/projects/test/apps/; done; done
