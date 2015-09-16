CC = g++
CFLAGS = -c -Wall -fpermissive

TARGETS = out/wqueue_test

SOURCES = \
		  src/thread.cpp \
		  src/main.cpp

OBJECTS	= $(SOURCES:.cpp=.o)

INCLUDES += -Iinc

LDFLAGS = -lpthread

$(TARGETS): $(OBJECTS) 
	$(CC) $(OBJECTS) -g -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf *.o out/wqueue_test src/*.o
	
