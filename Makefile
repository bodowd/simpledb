.PHONY: test

test:
	./build/bin/simpledb_test
	# clean up test files
	rm -rf filetest logtest
	


