#!/bin/bash
(>&2 echo "***MakeVersionFile.sh is DEPRECATED since v2.1d15 - use MakeVersionFile")
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

$MY_PATH/MakeVersionFile $@