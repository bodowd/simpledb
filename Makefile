.PHONY: test build clean all clean-students students

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

test: clean clean-students students
	./build/bin/simpledb_test
	$(MAKE) clean
	$(MAKE) clean-students


build:
	mkdir -p build
	cd build && cmake .. && make

all: build test

