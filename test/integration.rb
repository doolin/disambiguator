#!/usr/bin/env ruby

=begin
source ./bashcolors.sh
initializeANSI

echo
puts $cyanf"# Integration testing for disambiguator"
echo
=end

testdir      = "/data/patentdata/disambiguation/experiments/earth/berkeley"
finalfile    = "./data/berkeley/final_berkeley.txt"
rarenamefile = "./data/berkeley/rare_names_berkeley.txt"

#priorfiles = []
(1..6).each_with_index do |num|
  testfile  = "#{testdir}/prior_saved_#{num}.txt"
  priorfile = "./data/berkeley/prior_saved_#{num}.txt"
  results = `diff #{testfile} #{priorfile}`
  puts results unless results == ""
  #priorfiles << "./data/berkeley/prior_saved_#{num}.txt"
end

#puts priorfiles

#testval=`test_disambiguation $testdir $finalfile $rarenamefile`
#echo $cyanf"## Berkeley"
#echo -e $testval


=begin
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



echo $reset
=end
