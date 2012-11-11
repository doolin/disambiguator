#!/bin/sh

source ./bashcolors.sh
initializeANSI

./attribute
./engine
./jwcmp
./ratios
./ratiocomponent
./blocking
./compare
./similarity
./assigneecomparison
./coauthor
./strcmp95
./stringmanipulator
./training
./clusterinfo
./fetchrecords


echo "Dis? test"
`./distest`
# engineconfig spews a huge list of records
# to stdout.
echo "Engine config"
./engineconfig
echo "Rare names test"
`./rarenames`
echo "Typedefs test"
`./typedefs`
echo "Abbreviations test"
`./abbreviation`
echo "is_misspell test"
`./misspell`
echo "name_compare test"
`./namecompare`
echo "Record test"
`./record`

source ./integration.sh

