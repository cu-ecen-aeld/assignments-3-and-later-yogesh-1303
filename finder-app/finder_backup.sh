#!/bin/sh
filesdir=$1
searchstr=$2
#echo "${filesdir} world"
cd
if [ "$#" -ne 2 ] ; then
	echo "2 arguments required"
	echo "1: the file directory path"
	echo "2: the name of the file"
	exit 1
else	
	if [ -d "${filesdir}" ] ; then
		#cd ${filesdir}
		cd "${filesdir}"
		#if grep -q -x -r "${searchstr}" * ; then
		echo "number of lines found:"
		grep -x -r "${searchstr}" * | wc -l
		#else
		#	echo "match not found"
		#	exit 0
		#fi
	else
		echo "Argument 1 is not a directory"
		exit 1
	fi
fi

