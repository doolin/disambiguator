#!/bin/bash

function test_jones() {

  current_dir=`pwd`
  echo $current_dir

  jones_dir=/var/share/patentdata/disambiguation/test/synthetic/jones
  cd $jones_dir
  echo $jones_dir
  disambiguate engine.txt blocking6.txt > runtime.log

  cd $current_dir
  echo `pwd`
  testfile=$jones_dir"/final.txt"
  echo $testfile
  cat $testfile
  echo "0"
}

function test_eighteen() {

  current_dir=`pwd`
  echo $current_dir

  jones_dir=/var/share/patentdata/disambiguation/test/18
  cd $jones_dir
  echo $jones_dir
  disambiguate engine.txt blocking6.txt > runtime.log

  cd $current_dir
  echo `pwd`
  testfile=$jones_dir"/final.txt"
  echo $testfile
  cat $testfile
  # Do the diff here, capture the result to echo back
  echo "0"
}



testval=`test_jones`
echo "Jones result:  $testval"

testval=`test_eighteen`
echo "Eighteent:  $testval"

echo "arg 0:  $0"
echo "arg 1:  $1"
echo "arg 2:  $2"
echo "arg 3:  $3"

