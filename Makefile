.PHONY: test build all

TEST_DIRS = filetest logtest buffertest buffermanagertest transactiontest concurrencytest recordtest testfile tablescantest

test:
	# remove anything left over just in case
	rm -rf $(TEST_DIRS) 
	./build/bin/simpledb_test
	# clean up test files
	rm -rf $(TEST_DIRS) 

build:
	mkdir -p build
	cd build && cmake .. && make

all: build test

