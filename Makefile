
target = c-jni

objects = utils.o c-jni.o

rebuildables = $(objects) $(target)

all: $(target)
	echo "Success! All done."

#http://xahlee.info/UnixResource_dir/_/ldpath.html
$(target): $(objects)
	@g++ -L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/server/ \
	-L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/ \
	-Wl,-L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/ \
	-Wl,-L/usr/lib/jvm/java-7-oracle/jre/lib/amd64/server/ \
	-Wl,-R/usr/lib/jvm/java-7-oracle/jre/lib/amd64/ \
	-Wl,-R/usr/lib/jvm/java-7-oracle/jre/lib/amd64/server/ \
	-o $(target) $(objects) -Wl,-ljvm -ljvm
	
# Mode 1
#c-jni.o: jnitest.o
#	@g++ -O -c -I/usr/lib/jvm/java-7-oracle/include/ -I/usr/lib/jvm/java-7-oracle/include/linux/ -Wall -o c-jni.o jnitest.c
#	@g++ -S -I/usr/lib/jvm/java-7-oracle/include/ -I/usr/lib/jvm/java-7-oracle/include/linux/ -Wall -o c-jni.s jnitest.c

# Mode 2
%.o: %.c utils.h
	g++ -O -c -I/usr/lib/jvm/java-7-oracle/include/ -I/usr/lib/jvm/java-7-oracle/include/linux/ -Wall -o $@ $<

%.o: %.cpp utils.h
	g++ -O -c -I/usr/lib/jvm/java-7-oracle/include/ -I/usr/lib/jvm/java-7-oracle/include/linux/ -Wall -o $@ $<

clean:
	@rm $(objects) $(target) c-jni.s || true


