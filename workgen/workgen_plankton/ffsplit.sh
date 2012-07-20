#!/bin/bash

split_step=10
input="$1"

if [[ -f "$input" ]]; then
    file="$(basename $input)"
    echo file="$file"
    basename=${file%%.*}
    echo basename="$basename"
    extension=${file##*.}
    echo extension="$extension"
    path=${input/$file/}
    path=${path/user/tmp}
    echo path="$path"

    duration=$(ffmpeg -i "$input" 2>&1 | grep -E -o -e 'Duration: ..:..:..\...' | grep -E -o -e '(Duration: ){0}..:..:..\...')

    if [[ "$duration" -ne 1 ]]; then
        hours=${duration:0:2}
        minutes=${duration:3:2}
        seconds=${duration:6:2}

        length=$((10#$hours * 1440 + 10#$minutes * 60 + 10#$seconds))

        #mkdir -p "$path/$basename"
        prefix=0
        for offset in $(seq 0 $split_step $length); do
            if [ $offset -ne $length ]; then
                ((prefix++))
                #ffmpeg -loglevel quiet -i "$input" -t $split_step -sameq -ss $offset "$path/$basename/$basename-$prefix.$extension"
            fi
        done
        exit $prefix
    else
        echo Input file not found or broken
        exit -1
    fi
else
    echo Input file not found
    exit -1
fi
