
all: c-jni

#http://xahlee.info/UnixResource_dir/_/ldpath.html
c-jni: c-jni.o
	@g++ -L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/server/ \
	-L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/ \
	-Wl,-L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/ \
	-Wl,-L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/server/ \
	-Wl,-R/usr/lib/jvm/java-7-oracle/jre/lib/amd64/ \
	-Wl,-R/usr/lib/jvm/java-7-oracle/jre/lib/amd64/server/ \
	-o c-jni c-jni.o -Wl,-ljvm -ljvm
	
c-jni.o: jnitest.c
	@g++ -O -c -I/usr/lib/jvm/java-7-oracle/include/ -I/usr/lib/jvm/java-7-oracle/include/linux/ -Wall -o c-jni.o jnitest.c
	@g++ -S -I/usr/lib/jvm/java-7-oracle/include/ -I/usr/lib/jvm/java-7-oracle/include/linux/ -Wall -o c-jni.s jnitest.c

clean:
	@rm c-jni.o c-jni.s c-jni


