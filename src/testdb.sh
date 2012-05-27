#!/bin/sh

./txt2sqlite3 test.sqlite3 finaltest txt2sqlite3-test.txt urcn uicn
echo "Dumping test data from sqlite..."
sqlite3 -separator ',' test.sqlite3 "select * from finaltest" > tmp
echo "Diffing dumped with final test..."
diff tmp txt2sqlite3-test.final
rm tmp
#rm test.sqlite3
