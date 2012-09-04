#!/bin/bash

cd `dirname $0`
SCRIPTDIR=$PWD
USER_MODULES="$SCRIPTDIR/libSD $SCRIPTDIR/libFAT $SCRIPTDIR" make -f $LIB_MAPLE_HOME/Makefile $@
echo $SCRIPTDIR