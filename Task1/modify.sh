#!/bin/sh

help () {
	cat << EOT 
	Usage:
	modify [--help|-h|--h] prints help
	modify [-l] <file/dir name|names> -renames to lowercase
	modify [-u] <file/dir name|names> -renames to uppercase
	modify [-r] recoursive mode 
	modify [-s"SED_PATTERn"] <file/dir name|names> replacec in file names	
	modify [-h]
	
	by Pawel Polczynski
EOT
}

renameFiles () {

    currIFS= $IFS
    input="$1"              
    filePath=`dirname $1`
    fileName=`basename "$1"`
    IFS="/"    

    if [ "$opperation" = "0" ]; then
	        newFileName=`echo "$fileName" |  sed -s "$sedPattern"` 
    elif [ "$opperation" = "1" ]; then    
        	newFileName=`echo $fileName | tr '[A-Z]' '[a-z]'`
    elif [ "$opperation" = "2" ]; then  
	        newFileName=`echo $fileName | tr '[a-z]' '[A-Z]'`
    fi
     
 
    if [ "$fileName" != "$newFileName" ] && ! [ -e "$filePath/$newFileName" ]; 
	then     
            mv "$input" "$filePath/$newFileName"
    fi
    IFS=$currIFS         
}

toUppercase="false"
toLowercase="false"
recursion="false"
sedPattern="null"

if [ "$#" -eq "0" ];
	then
		echo "No arguments provided"
		help
		exit 1
	fi

while [ "$#" -ne "0" ]; 
	do
		case "$1" in
			"-h")
				help
				exit 0
				;;
			"--help")
				help
				exit 0
				;;
			"--h")
				help
				exit 0
				;;
			"-r")
				recursion="true"
				shift
				;;
			"-l")
				toLowercase="true"
				shift
				;;
			"-u")
				toUppercase="true"
				shift
				;;

			"-s"*)
				sedPattern=`echo $1 | sed "s/-s//"`
				shift
				;;							
			*)	
				if [ $toUppercase = "true" ] || [ "$toLowercase" = "true" ] || [ "$sedPattern" != "null" ] ; then
						break			
				fi	
				;;
			 
	esac	
done

if [ "$toLowercase" = "true" ] && [ "$toUppercase" = "true" ]; then			#if both -l and -u set
	echo "Invalid instruction detected"
	echo "can not convert to Upper and Lower case at the same time"
	help
	exit 2
fi

opperation="-1"
if [ "$sedPattern" != "null" ]; then			
	opperation="0" 
elif [ $toLowercase = "true" ]; then
	opperation="1"
elif [ $toUppercase = "true" ]; then
	opperation="2"
fi
if [ $opperation = "-1" ]; then
		echo "No arguments."
		help
		exit 3
fi


if [ $recursion = "true" ]; then			

	if [ "$toUppercase" = "true" ]; then
		flag="-u"
	else
		flag="-l"
	fi

	if [ ! -d "$1" ]; then
		echo "$1" is not a directory		
		help		
		exit 0	
	fi

    hasNestedDir=`ls -al $1 | grep '^d' | grep -v [.-..]`
    if [ "$hasNestedDir" != "" ];then  
        dir=`ls -d $1*/`  	
		for dir in $dirs
		do	
			$0 -r $flag $dir
		done
	fi

	isDirEmpty=`ls -a $1 | grep -v '^\.\{1,2\}$'`	#check if folder is empty
	if [ "$isDirEmpty" = "" ]; then
		$0 $flag $1
	else 	
		$0 $flag $1*
	fi
	exit 0
fi


while [ "$#" -ne "0" ]; 
	do
		if [ -e "$1" ]; then 
			renameFiles "$1"	
		else 
			echo "Submited file \"$1\" does not exist" 	
		fi
		shift
	done
exit 0





 
