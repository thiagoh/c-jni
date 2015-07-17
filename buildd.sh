#!/bin/bash

#g++ -I/usr/lib/jvm/java-7-oracle/include/ -I/usr/lib/jvm/java-7-oracle/include/linux/ -L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/server/ -L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/ jnitest.c -ljvm

g++ -S -I/usr/lib/jvm/java-7-oracle/include/ -I/usr/lib/jvm/java-7-oracle/include/linux/ -Wall jnitest.c

g++ -c -I/usr/lib/jvm/java-7-oracle/include/ -I/usr/lib/jvm/java-7-oracle/include/linux/ -Wall jnitest.c

#http://xahlee.info/UnixResource_dir/_/ldpath.html

g++ -L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/server/ -L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/ -Wl,-L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/ -Wl,-L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/server/ -Wl,-R/usr/lib/jvm/java-7-oracle/jre/lib/amd64/ -Wl,-R/usr/lib/jvm/java-7-oracle/jre/lib/amd64/server/ -o jnitest jnitest.o -Wl,-ljvm -ljvm

