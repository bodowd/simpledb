.PHONY: test build clean all

TEST_DIRS = filetest logtest buffertest buffermanagertest transactiontest \
concurrencytest recordtest testfile tablescantest tabletest tablemanagertest \
catalogtest metadatamanagertest metadatamanagertest2 scantest scantest2 tokenizertest \
parsertest lexertest plannertest1 indexretrievaltest indexupdatetest

clean: 
	rm -rf $(TEST_DIRS)

clean-students:
	rm -rf students

students:
	./build/bin/createstudentdb

test:
	# remove anything left over just in case
	rm -rf $(TEST_DIRS) 
	./build/bin/simpledb_test
	# clean up test files
	rm -rf $(TEST_DIRS) 

build:
	mkdir -p build
	cd build && cmake .. && make

all: build test clean

