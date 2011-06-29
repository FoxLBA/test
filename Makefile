#!/usr/bin/make -f

include Makefile.incl

TARGET_DIRS = apps/test-text apps/test-ffmpeg assimilator/assimilator_text assimilator/assimilator_ffmpeg validator workgen/workgen_text workgen/workgen_ffmpeg

all:
	for d in $(TARGET_DIRS); do make -C $$d; done

clean:
	for d in $(TARGET_DIRS); do make -C $$d clean; done


serverput sp:
	rsync -rchh --delete-after --progress . $(SERVER_PATH)/

serverget sg:
	rsync -rchh --delete-after --progress $(SERVER_PATH)/ .

serverdryput sdp:
	rsync -rchhn --delete-after --progress . $(SERVER_PATH)/

serverdryget sdg:
	rsync -rchhn --delete-after --progress $(SERVER_PATH)/ .
