#!/bin/bash

source ./bashcolors.sh
initializeANSI

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
  echo "diff resulted in $results"
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
  #echo "diff resulted in $results"
  echo ${#results}
  if [[ $results ]] ; then
    echo "diff resulted in $results"
    echo $redf"failed"
  else
    echo $greenf"passed"
  fi
}


function test_disambiguation() {

  current_dir=`pwd`
  dir=$1 
  cd $dir

  disambiguate engine.txt blocking6.txt > runtime.log

  cd $current_dir
  output=$dir"/final.txt"
  testfile=$2
  results=`diff ${output} ${testfile}`
  #echo ${#results}
  if [[ $results ]] ; then
    echo $redf"failed: diff results in $results"
  else
    echo $greenf"passed"
  fi
}


testdir=/var/share/patentdata/disambiguation/test/synthetic/jones
testfile="./data/final_jones.txt"
testval=`test_disambiguation $testdir $testfile`
echo $cyanf"Jones:  $testval"

testdir=/var/share/patentdata/disambiguation/test/18
testfile="./data/final_eighteen.txt"
testval=`test_disambiguation $testdir $testfile`
echo $yellowyf"Eighteen: $testval"

testdir=/var/share/patentdata/disambiguation/experiments/earth/berkeley
testfile="./data/final_berkeley.txt"
testval=`test_disambiguation $testdir $testfile`
echo $orangeyf"Berkeley: $testval"


#echo "arg 0:  $0"
#echo "arg 1:  $1"
#echo "arg 2:  $2"
#echo "arg 3:  $3"

echo $reset
