#!/bin/bash

for i in `seq 1 34`; do
  mvn exec:java -Dexec.args="-t $i -n 20000000 -v 1 -r 5  -w 3 --file /home/johl/md5_fine_java.passes"
done


for i in `seq 1 34`; do
  mvn exec:java -Dexec.args="-t $i -n 10000000 -v 5 -r 5  -w 3 --file /home/johl/md5_medium-fine_java.passes"
done


for i in `seq 1 34`; do
  mvn exec:java -Dexec.args="-t $i -n 5000000 -v 11 -r 5  -w 3 --file /home/johl/md5_medium_java.passes"
done


for i in `seq 1 34`; do
  mvn exec:java -Dexec.args="-t $i -n 1000000 -v 53 -r 5  -w 3 --file /home/johl/md5_coarse_java.passes"
done
