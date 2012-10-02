#!/usr/bin/env ruby

require './termcolors'

class IntegrationTest

  include TermColors

  #@@finalfile    = "./data/berkeley/final_berkeley.txt"
  #@@rarenamefile = "./data/berkeley/rare_names_berkeley.txt"

  @@testdir      = "/data/patentdata/disambiguation/experiments/earth/berkeley"

end


class Prior < IntegrationTest

  def initialize
    print "## #{@colors[116]} Integration testing for priors", RESET, "\n"
  end

  def test_priors
    (1..6).each_with_index do |num|
      testfile  = "#{@@testdir}/prior_saved_#{num}.txt"
      priorfile = "./data/berkeley/prior_saved_#{num}.txt"
      results = `diff -y --suppress-common-lines #{testfile} #{priorfile}`
      puts results unless results == ""
    end
  end
end

class Tset < IntegrationTest

  def initialize
    print "## #{@@colors[116]} Integration testing for tsets", RESET, "\n"
  end

  def test_tsets
    (2..2).each_with_index do |num|
      testfile  = "#{@@testdir}/tset05_#{num}.txt"
      priorfile = "./data/berkeley/tset05_#{num}.txt"
      results = `diff -y --suppress-common-lines #{testfile} #{priorfile}`
      puts results unless results == ""
    end
  end

end


tset = Tset.new
tset.test_tsets
