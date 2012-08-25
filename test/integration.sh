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

  #echo Directory: $1
  current_dir=`pwd`
  dir=$1 #/var/share/patentdata/disambiguation/test/18
  cd $dir

  disambiguate engine.txt blocking6.txt > runtime.log

  cd $current_dir
  output=$dir"/final.txt"
  testfile="./data/final_berkeley.txt"
  results=`diff ${output} ${testfile}`
  echo ${#results}
  if [ ${#results} > 0 ] ; then
    echo "diff resulted in $results"
    echo $redf"failed"
  else
    echo $greenf"passed"
  fi
}


testval=`test_jones foo`
echo $cyanf"Jones:  $testval"
testval=`test_eighteen`
echo $yellowyf"Eighteen: $testval"

testdir=/var/share/patentdata/disambiguation/experiments/earth/berkeley
testval=`test_disambiguation $testdir`
echo $orangeyf"Berkeley: $testval"


#echo "arg 0:  $0"
#echo "arg 1:  $1"
#echo "arg 2:  $2"
#echo "arg 3:  $3"

echo $reset
