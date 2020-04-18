#! /bin/sh
./mapreduce map ./map_script in.txt temp.txt
./mapreduce reduce ./reduce_script temp.txt out.txt

