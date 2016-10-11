#!/bin/bash

for i in `seq 1 34`; do
  mvn exec:java -Dexec.args="-t $i -n 10000000 -v 0 -r 5  -w 3 --file /home/johl/md5_fine_java.passes"
done


for i in `seq 1 34`; do
  mvn exec:java -Dexec.args="-t $i -n 5000000 -v 2 -r 5  -w 3 --file /home/johl/md5_medium-fine_java.passes"
done


for i in `seq 1 34`; do
  mvn exec:java -Dexec.args="-t $i -n 2000000 -v 7 -r 5  -w 3 --file /home/johl/md5_medium_java.passes"
done


for i in `seq 1 34`; do
  mvn exec:java -Dexec.args="-t $i -n 1000000 -v 18 -r 5  -w 3 --file /home/johl/md5_coarse_java.passes"
done
