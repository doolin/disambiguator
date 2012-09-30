#!/bin/bash

source ./bashcolors.sh
initializeANSI

echo
echo $cyanf"# Integration testing for disambiguator"
echo

function test_disambiguation() {

  current_dir=`pwd`
  dir=$1 
  cd $dir

  ~/src/disambiguator/src/disambiguate engine.txt blocking6.txt > runtime.log

  cd $current_dir
  output=$dir"/final.txt"
  testfile=$2
  results=`diff ${output} ${testfile}`
  if [[ $results ]] ; then
    echo $redf"failed final: diff results in $results"
  else
    echo $greenf"passed final"
  fi



  if [[ $3 ]] ; then
    echo "\n"
    rare=$dir"/Rare_Names.txt"
    results=`diff ${rare} $3`
    if [[ $results ]] ; then
      echo $redf"failed rare names: diff results in $results"
    else
      echo $greenf"passed rare names"
    fi
  fi
}


# TODO: Create an array for the test directories and files,
# then loop it to run the tests.
testdir=/data/patentdata/disambiguation/test/synthetic/jones
testfile="./data/final_jones.txt"
testval=`test_disambiguation $testdir $testfile`
echo $cyanf"## Jones"
echo $testval

echo

testdir=/data/patentdata/disambiguation/test/18
testfile="./data/final_eighteen.txt"
testval=`test_disambiguation $testdir $testfile`
echo $cyanf"## Eighteen"
echo $testval

echo

testdir=/data/patentdata/disambiguation/experiments/earth/berkeley
finalfile="./data/berkeley/final_berkeley.txt"
rarenamefile="./data/berkeley/rare_names_berkeley.txt"
testval=`test_disambiguation $testdir $finalfile $rarenamefile`
echo $cyanf"## Berkeley"
echo -e $testval


./integration.rb


echo $reset
