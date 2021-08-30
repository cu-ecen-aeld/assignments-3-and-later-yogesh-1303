#!/bin/bash
filesdir=$1
searchstr=$2
#numfiles=$(sudo find "${filesdir}" -type f | wc -l)
cd
if [ "$#" -ne 2 ] ; then
	echo "2 arguments required"
	echo "1: the file directory path"
	echo "2: the name of the file"
	exit 1
else	
	if [ -d "${filesdir}" ] ; then
		cd "${filesdir}"
		numlines=$(sudo grep -x -r "${searchstr}" * | wc -l)
		#numfiles=$(find -L . -type f -exec grep -x -r -l "${searchstr}" {} + | wc -l)
		numfiles=$(grep -l -Rx "${filesdir}" -e "${searchstr}" | wc -l)
		#numfiles=$(ls -alR | grep -x -r "${searchstr}" | wc -l)
		echo "The number of files are ${numfiles} and the number of matching lines are ${numlines}"
	else
		echo "Argument 1 is not a directory"
		exit 1
	fi
fi

