#! /bin/bash
set -eou pipefail

# the time of the profile
NOW=`date +%Y%m%d-%H%M%S`

## Adjust these variables to match your installation
# CUPTI should be in the LD_LIBRARY_PATH

if [ -z "${OPENTRACING_LIB+xxx}" ]; then 
  export OPENTRACING_LIB="$HOME/software/opentracing-cpp/lib";
fi
if [ -z "${ZIPKIN_LIB+xxx}" ]; then 
  export ZIPKIN_LIB="$HOME/software/zipkin-cpp-opentracing/lib";
fi

# where to look for cprof/profiler
if [ -z "${CPROF_ROOT+xxx}" ]; then 
  export CPROF_ROOT="$HOME/repos/cupti"; # not set at all
fi

# Check that libcprof.so exists
LIBCPROF="$CPROF_ROOT/cprof/lib/libcprof.so"
if [ ! -f "$LIBCPROF" ]; then
    echo "$LIBCPROF" "not found! try"
    echo "make -C $CPROF_ROOT/cprof"
    exit -1
fi

# Check that libprofiler.so exists
LIBPROFILER="$CPROF_ROOT/profiler/lib/libprofiler.so"
if [ ! -f "$LIBPROFILER" ]; then
    echo "$LIBPROFILER" "not found! try"
    echo "make -C $CPROF_ROOT/profiler"
    exit -1
fi


## Add the libraries libprofiler.so depends on to the load library path
export LD_LIBRARY_PATH="/usr/local/cuda/extras/CUPTI/lib64:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="$OPENTRACING_LIB:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="$ZIPKIN_LIB:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="$CPROF_ROOT/cprof/lib:$LD_LIBRARY_PATH"

## Control some profiling parameters.

# default output file
export CPROF_OUT="$NOW\_output.cprof"
#export CPROF_ERR="err.cprof"


# endpoint for tracing
export CPROF_ENABLE_ZIPKIN=0
export CPROF_ZIPKIN_HOST=34.215.126.137
export CPROF_ZIPKIN_PORT=9411

## Run the provided program. For example
#   ./env.sh examples/samples/vectorAdd/vec

if [ -z "${LD_PRELOAD+xxx}" ]; then 
  LD_PRELOAD="$CPROF_ROOT/profiler/lib/libprofiler.so" $@; # unset
else
  echo "Error: LD_PRELOAD is set"
fi
