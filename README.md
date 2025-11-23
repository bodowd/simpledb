This is an educational project to explore concepts around relational databases and to build a simple, functional relational database management system.
The project follows the book Database Design and Implementation by Edward Sciore.
The code is a port of Edward Sciore's SimpleDB from Java to C++, and is heavily inspired by rotaki's version.

# Getting started

## Build

```sh
mkdir build
cd build
cmake ..
make
```

## Run tests

To run tests, run

```sh
# this removes the old test directories first -- if the old test directories are not removed, the tests
# won't be starting from the expected state
make test
```
