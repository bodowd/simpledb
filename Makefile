.PHONY: test build all

test:
	# remove anything left over just in case
	rm -rf filetest logtest
	./build/bin/simpledb_test
	# clean up test files
	rm -rf filetest logtest buffertest

build:
	mkdir -p build
	cd build && cmake .. && make

all: build test

