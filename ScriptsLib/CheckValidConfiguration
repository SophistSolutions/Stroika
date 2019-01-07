#!/bin/bash
get_script_dir () {
     #https://www.ostricher.com/2014/10/the-right-way-to-get-the-directory-of-a-bash-script/
     SOURCE="${BASH_SOURCE[0]}"
     # While $SOURCE is a symlink, resolve it
     while [ -h "$SOURCE" ]; do
          DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
          SOURCE="$( readlink "$SOURCE" )"
          # If $SOURCE was a relative symlink (so no "/" as prefix, need to resolve it relative to the symlink base directory
          [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
     done
     DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
     echo "$DIR"
}
MY_PATH=$(get_script_dir)
CONFIGURATION=$1
if [ ! -e "$MY_PATH/../ConfigurationFiles/$CONFIGURATION.xml" ] ; then
	echo -n "\"$CONFIGURATION\" is not a valid configuration; valid configurations include: [ " && $MY_PATH/GetConfigurations && echo "]";
	exit 1;
fi
