#!/usr/bin/make -f

include Makefile.incl

TARGET_DIRS = apps/test-text apps/test-ffmpeg assimilator/assimilator_text assimilator/assimilator_ffmpeg validator workgen/workgen_text workgen/workgen_ffmpeg

all:
	for d in $(TARGET_DIRS); do make -C $$d; done

clean:
	for d in $(TARGET_DIRS); do make -C $$d clean; done


put p:
	rsync -rchh --delete-after --progress . $(SERVER_PATH)/

get g:
	rsync -rchh --delete-after --progress $(SERVER_PATH)/ .

dryput dp:
	rsync -rchhn --delete-after --progress . $(SERVER_PATH)/

dryget dg:
	rsync -rchhn --delete-after --progress $(SERVER_PATH)/ .
