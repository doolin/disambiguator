#!/bin/sh

sqlite3 -separator ',' test.sqlite3 "select * from finaltest" > tmp
diff tmp txt2sqlite3-test.final
rm tmp
