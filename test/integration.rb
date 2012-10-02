#!/usr/bin/env ruby

require './termcolors'

class IntegrationTest

  include TermColors

  #@@finalfile    = "./data/berkeley/final_berkeley.txt"
  #@@rarenamefile = "./data/berkeley/rare_names_berkeley.txt"

  @@testdir      = "/data/patentdata/disambiguation/experiments/earth/berkeley"

  def print_headline(testname)
    color = @@colors[116]
    print "#{color}## Integration testing for #{testname}", RESET, "\n"
  end

  def print_passed(testname)
    color = @@colors[116]
    print "  #{color} Passed: #{testname}", RESET, "\n"
  end

  def print_failed(testname)
    color = @@colors[116]
    print "  #{color} Failed: #{testname}", RESET, "\n"
  end
end


class Prior < IntegrationTest

  def initialize
    print_headline("priors")
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
    print_headline("tsets")
  end

  def test_tsets
    (2..2).each_with_index do |num|
      tset = "tset05_#{num}"
      testfile  = "#{@@testdir}/#{tset}.txt"
      priorfile = "./data/berkeley/#{tset}.txt"
      results = `diff -y --suppress-common-lines #{testfile} #{priorfile}`
      if results
        print_passed(tset)
      else
        print_failed(tset)
      end
    end
  end

end


prior = Prior.new
tset = Tset.new
tset.test_tsets
