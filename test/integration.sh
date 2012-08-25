#!/bin/bash

function test_jones() {

  current_dir=`pwd`
  dir=/var/share/patentdata/disambiguation/test/synthetic/jones
  cd $dir

  disambiguate engine.txt blocking6.txt > runtime.log

  cd $current_dir
  #echo `pwd`
  output=$dir"/final.txt"
  testfile="./data/final_jones.txt"
  results=`diff ${output} ${testfile}`
  echo "jones diff resulted in $results"
}

function test_eighteen() {

  current_dir=`pwd`
  dir=/var/share/patentdata/disambiguation/test/18
  cd $dir

  disambiguate engine.txt blocking6.txt > runtime.log

  cd $current_dir
  output=$dir"/final.txt"
  testfile="./data/final_eighteen.txt"
  results=`diff ${output} ${testfile}`
  echo "eighteen diff resulted in $results"
}

source ./bashcolors.sh
initializeANSI


testval=`test_jones`
echo $cyanf"Jones result:  $testval"

testval=`test_eighteen`
echo $orangeyf"Eighteen: $testval"

#echo "arg 0:  $0"
#echo "arg 1:  $1"
#echo "arg 2:  $2"
#echo "arg 3:  $3"

echo $reset
