#!/bin/bash
PROG=$1

function _clear()
{
	echo "Cleaning up"
	# $PROG " "
}

trap _clear EXIT

#main
for i in $(seq 9 -1 0); do
	$PROG $i
done

#end

