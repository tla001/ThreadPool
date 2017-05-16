#!/bin/bash

DEST_PATH="./inc/"

headers=`find . -name *.h`
#echo $headers

for file in $headers
do
	#echo $file
	temp=${file#*/}
	temp=${temp#*/}
	dstfile=$DEST_PATH$temp
	dstpath=${dstfile%/*}
	#echo $dstpath
	[ -d $dstpath ] || mkdir -p  $dstpath
	
	cp -R $file $dstfile

done
