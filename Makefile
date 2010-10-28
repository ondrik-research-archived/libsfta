# run make to process this Makefile

.PHONY: all

all:
	autoreconf --install && ./configure && make
