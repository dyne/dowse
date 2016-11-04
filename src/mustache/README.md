# Overview

mustache-c is a basic implementation of mustache templating in pure C.

# Features

Supported:

* Sleek pure C API
* Prerendering for better speed
* Sections and variables

Not supported:

* Partials
* HTML escaping
* Custom delimiters

# Installation

Simply use the ```configure``` script to compile and install mustache-c:

```shell
$ ./configure --prefix=/usr
$ make
$ sudo make install
```

mustache-c requires ```flex```, ```bison``` and reasonable modern C
library that provides ```stdint.h```. If you wish to build the doxygen
documentation you will also need ```doxygen```.

mustache-c provides a ```pkg-config``` file with which you can determine
libraries and include paths:

```shell
$ pkg-config --libs mustache_c-1.0
$ pkg-config --cflags mustache_c-1.0
```

This also allows mustache-c to be picked up by ```cmake```:

```cmake
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(MUSTACHEC REQUIRED mustache_c-1.0)
```

# Usage

There is a [test](https://github.com/x86-64/mustache-c/blob/master/test/test.c)
program available in the source which shows the basic usage of mustache-c.

# API Documentation

See [this link](http://x86-64.github.com/mustache-c/) for the Doxygen
documentation of the API.
