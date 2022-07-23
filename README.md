# uset
## Introduction
`uset` is a micro(µ) implementation of *set* data structure using RB tree in C. The RB tree programming is inspired by the well-known textbook, [Introduction to Algorithms](https://en.wikipedia.org/wiki/Introduction_to_Algorithms).

## Features
`uset` supports some basic operations of the *set* data structure.
- `uset_create`: Create an `uset` data structure
- `uset_clear`: Clear all elements in the `uset` data structure
- `uset_add`: Add an element to the `uset` data structure
- `uset_delete`: Delete an element in the `uset` data structure
- `uset_has`: Exam if a certain element is in the `uset` data structure

## Build and test
Use `make` to build the library and test program.
```
$ make
```
Test the built library
```
$ make test
```