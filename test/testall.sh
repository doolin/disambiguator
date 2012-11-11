#!/bin/sh


source ./bashcolors.sh
initializeANSI

./clusterinfo

echo "Fetch records test"
./fetchrecords

echo "Attribute test"
./attribute

echo "Record test"
`./record`

echo "String Manipulator test"
`./stringmanipulator`

echo "Dis? test"
`./distest`

# engineconfig spews a huge list of records
# to stdout. 
#`engineconfig`

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

echo "strcmp95 test"
`./strcmp95`

echo "training test"
./training

echo "jwcmp test"
./jwcmp

echo "engine test"
./engine
./ratios
./ratiocomponent
./blocking

./compare
./similarity
./assigneecomparison
./coauthor

echo "integration test"
source ./integration.sh

