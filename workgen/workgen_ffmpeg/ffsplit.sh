#!/bin/bash

input="$1"
file="$(basename $input)"
echo file="$file"
basename=${file%%.*}
echo basename="$basename"
extension=${file##*.}
echo extension="$extension"
path=${input/$file/}
echo path="$path"

duration=$(ffmpeg -i "$input" 2>&1  | grep -E -o -e 'Duration: ..:..:..\...' | grep -E -o -e '(Duration: ){0}..:..:..\...')
#echo $duration

hours=${duration:0:2}
minutes=${duration:3:2}
seconds=${duration:6:2}
#echo $hours $minutes $seconds

length=$((hours*1440 + minutes*60 + seconds))
#echo $length

mkdir "$path/$basename"
prefix=0
for offset in $(seq 0 10 $length); do
    if [ $offset -ne $length ]; then
        ((prefix++))
        ffmpeg -i "$input" -t 10 -sameq -ss $offset "$path/$basename/$basename-$prefix.$extension"
    fi
done
exit $prefix
