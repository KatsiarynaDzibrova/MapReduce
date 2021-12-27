# MapReduce

Immitation of MapReduce programming model. Model is used for the problem of splitting numbers into intervals.

There is also scripts for generating test samples and for visualising results. 

./generate -n 200000 -d "in.txt" -b 100  OR ./generate -n 200000 -d "in.txt"  

./mapreduce map ./map_script in.txt temp.txt

./mapreduce reduce ./reduce_script temp.txt out.txt
