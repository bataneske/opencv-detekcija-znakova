#!/bin/bash

if [[ ! -f build/configured ]]
then
	pushd ../
	./flow.sh
	x=$?
	popd
	if [[ "$x" == 0 ]]
	then
		./waf configure --visualizer-prefix=$HOME/.local
		if [[ "$?" == 0 ]]
		then
			touch build/configured
		else
			exit $?
		fi
	fi
fi

pushd ../
./waf build install
x=$?
popd
if [[ "$x" == 0 ]]
then
	./waf build run
	exit $?
else
	exit $x
fi
