#!/bin/bash

#link & mount point checker

TRUE=1
FALSE=0

#
# Step1: Created folder for trash.
#
TRASH_DIR="/home/`whoami`/.trash"

create_trash()
{
	if [ ! -d $TRASH_DIR ]; then
	    mkdir -p $TRASH_DIR
	    chmod 777 $TRASH_DIR
    fi
    if [ $? -ne 0 ]; then
	    echo "Error:failes to create $TRASH_DIR"
    fi
}

#
#Step2: check the file is mount point or link point.
#

_checker_linkpoint()
{
	file_name=$1
	if [ -h $file_name ]; then
		echo -e "\033[31mThe folder $file_name is a link point.\033[0m"
		return $TRUE
	else
		return $FALSE
	fi
}

_checker_mountpoint()
{
	file_name=$1
    mountpoint -q $file_name
    if [ $? -eq 0 ]; then
    	echo -e "\033[31mThe folder $file_name is a mount point.\033[0m"
		return $TRUE
	else
		return $FALSE
	fi
}

checker_mountpoint_or_linkpoint()
{
    file_name=$1
    _checker_linkpoint $file_name
   
   _checker_mountpoint $file_name 
}

loop_down_checker()
{
	local file_path=$1
    
	if [ -d $file_path ]; then
	   for sub_file_path in `ls -a $file_path`; do
           if [ $sub_file_path='.' ] || [ $sub_file_path='..' ]; then
           	  break
           fi
           file_name=$file_path/$sub_file_path
           checker_mountpoint_or_linkpoint $file_name         
           loop_down_checker $file_name
	   done
    else
	   _checker_linkpoint $file_path
	   return
    fi
}

loop_up_checker()
{
	local file_name=$1
	if [ $file_name = '/' ]; then
		return
	fi
	checker_mountpoint_or_linkpoint $file_name
    loop_up_checker `dirname $file_name`
}

main()
{
	create_trash
	FILE_NAME=$1
	if [ ${FILE_NAME:0:1} != '/' ]; then
       FILE_NAME=$PWD/$FILE_NAME
    fi
    if [ ${FILE_NAME: -1} = '/' ]; then
	   FILE_NAME=${FILE_NAME%?}
    fi

    loop_up_checker $FILE_NAME
    loop_down_checker $FILE_NAME
}

main $*
echo "Done!"