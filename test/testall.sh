#!/bin/sh

echo "Attribute test"
`./attribute`

echo "Record test"
`./record`

echo "String Manipulator test"
`./stringmanipulator`

echo "Blocking test"
`./blocking`

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

echo "jwcmp test"
`./jwcmp`

