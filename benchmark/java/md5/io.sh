#!/bin/bash

#for i in `seq 1 34`; do
#  mvn exec:java  -Dexec.args="-t $i -n 1000000 -v 1024 -r 5  -w 3 --file /home/johl/io_fine_java.passes"
#done


for i in `seq 1 34`; do
  mvn exec:java -Dexec.args="-t $i -n 100000 -v 131072 -r 5  -w 1 --file /home/johl/io_medium-fine_java.passes"
done


for i in `seq 1 34`; do
  mvn exec:java -Dexec.args="-t $i -n 20000 -v 1048576 -r 5  -w 1 --file /home/johl/io_medium_java.passes"
done


for i in `seq 1 34`; do
  mvn exec:java -Dexec.args="-t $i -n 2000 -v 10485760 -r 5  -w 1 --file /home/johl/io_coarse_java.passes"
done
