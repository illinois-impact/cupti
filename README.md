# CUPTI

| master | develop |
|--------|---------|
| coming soon... | [![Build Status](https://travis-ci.org/cwpearson/cupti.svg?branch=develop)](https://travis-ci.org/cwpearson/cupti)

## Setup

Install some dependencies

    sudo apt install libnuma-dev libboost-all-dev libcurl4-openssl-dev cmake cppcheck

Install CUDA and CUDNN.

Install opentracing-cpp and and zipkin-opentracing-cpp.

1. opentracing-cpp

```bash
    git clone https://github.com/opentracing/opentracing-cpp.git \
      && cd opentracing-cpp \
      && mkdir build \
      && cd build \
      && cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$HOME/software/opentracing-cpp \
      && make \
      && make install
```


2. zipkin-opentracing-cpp

```bash
    git clone https://github.com/rnburn/zipkin-cpp-opentracing.git \
      && cd zipkin-cpp-opentracing \
      && mkdir build \
      && cd build \
      && cmake .. -DCMAKE_INSTALL_TYPE=Debug \
                  -DCMAKE_INSTALL_PREFIX=$HOME/software/zipkin-cpp-opentracing \
                  -DOPENTRACING_INCLUDE_DIR=$HOME/software/opentracing-cpp/include \
                  -DOPENTRACING_LIB=$HOME/software/opentracing-cpp/lib/libopentracing.so \
      && make  \
      && sudo make install
```


If you wish to install either opentracing-cpp or zipkin-opentracing-cpp in a different
location you can use the DCMAKE_INSTALL_PREFIX flag to change the installation location like so:

```bash
-DCMAKE_INSTALL_PREFIX=/custom/installation/path
```

## Building cprof

The cprof library contains functionality to model the system and cuda driver state.

Create a `Makefile.config` in `cprof` that matches your environment.

    cd cprof
    cp Makefile.config.example Makefile.config
    make tests

## Building profiler

The profiler uses cprof to model the system state, and uses CUPTI and LD\_PRELOAD to observe the system.

Create a `Makefile.config` in `profiler` that matches your environment.

    cd profiler
    cp Makefile.config.example Makefile.config
    make

## Using the profiler

Make sure the CUPTI library is in your `LD_LIBRARY_PATH`. For example:

    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/cuda/extras/CUPTI/lib64"

Modify `env.sh` to point `CPROF_ROOT` to wherever you checked out cupti. For example

    export CPROF_ROOT="/home/pearson/cupti"

Build the profiling library (`prof.so`).

    make

## Run on a CUDA application

Make sure your CUDA application is not statically-linked, which is the default when you are building your own CUDA code.

This will record data by appending to an output.cprof file, so usually remove that file first. `./env.sh` sets up the `LD_PRELOAD` environment and invokes your app.

    rm -f output.cprof
    ./env.sh <your app>

Do something with the result:

    cprof2<something>.py

Other info

`env.sh` sets `LD_PRELOAD` to load the profiling library and its dependences.

## Run with gdb

Build the library with debug symbols.

Modify `env.sh` so that the final line calls `gdb` before the arguments, like so:

    LD_PRELOAD="$LD_PRELOAD:$CPROF_ROOT/lib/libcprof.so" gdb $@

## pycprof for looking at results

Install [cwpearson/pygraphml](https://github.com/cwpearson/pygraphml)

```bash
    # Remove existing pygraphml
    pip uninstall pygraphml
    git clone git@github.com:cwpearson/pygraphml.git
    cd pygraphml
    pip install --user -e .
    cd ..
```

Install [cwpearson/pycprof](https://github.com/cwpearson/pycprof)

```bash
    # Remove existing pygraphml
    git clone git@github.com:cwpearson/pycprof.git
    cd pycprof
    pip install --user -e .
    cd ..
```

