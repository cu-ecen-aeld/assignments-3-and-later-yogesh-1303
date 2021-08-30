#!/bin/sh
writefile=$1
writestr=$2
cd
if [ "$#" -ne 2 ] ; then
	echo "2 arguments required"
	echo "1: the complete file path"
	echo "2: the string to be written"
	exit 1
else
	if  touch "${writefile}" ; then
		echo "${writestr}" > "${writefile}"
	else
		echo "file cannot be created"
		exit 1
	fi
fi
