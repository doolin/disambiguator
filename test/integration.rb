#!/usr/bin/env ruby

require './termcolors'

print "## #{@colors[116]} Integration testing for priors", RESET, "\n"

testdir      = "/data/patentdata/disambiguation/experiments/earth/berkeley"
finalfile    = "./data/berkeley/final_berkeley.txt"
rarenamefile = "./data/berkeley/rare_names_berkeley.txt"

(1..6).each_with_index do |num|
  testfile  = "#{testdir}/prior_saved_#{num}.txt"
  priorfile = "./data/berkeley/prior_saved_#{num}.txt"
  results = `diff -y --suppress-common-lines #{testfile} #{priorfile}`
  puts results unless results == ""
end

