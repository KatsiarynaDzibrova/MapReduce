#! /bin/sh
./mapreduce map ./map_script in.txt out.txt
./mapreduce reduce ./reduce_script out.txt final.txt

