#!/bin/bash

source ./bashcolors.sh
initializeANSI


function test_disambiguation() {

  current_dir=`pwd`
  dir=$1 
  cd $dir

  ~/src/disambiguator/src/disambiguate engine.txt blocking6.txt > runtime.log

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

# TODO: Create an array for the test directories and files,
# then loop it to run the tests.
testdir=/var/share/patentdata/disambiguation/test/synthetic/jones
testfile="./data/final_jones.txt"
testval=`test_disambiguation $testdir $testfile`
echo $cyanf"Jones:  $testval"

testdir=/var/share/patentdata/disambiguation/test/18
testfile="./data/final_eighteen.txt"
testval=`test_disambiguation $testdir $testfile`
echo $cyanf"Eighteen: $testval"

testdir=/var/share/patentdata/disambiguation/experiments/earth/berkeley
testfile="./data/final_berkeley.txt"
testval=`test_disambiguation $testdir $testfile`
echo $cyanf"Berkeley: $testval"


#echo "arg 0:  $0"
#echo "arg 1:  $1"
#echo "arg 2:  $2"
#echo "arg 3:  $3"

echo $reset
