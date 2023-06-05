CC = g++
CFLAGS = -g -Os -Wall -Wno-deprecated -fPIC -pipe -std=c++11 -pthread -std=c++14 -m64
ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)

TARGET = CORSIKA
SOURCES = CORSIKA.cc
OBJECTS = $(SOURCES:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CC) $(CFLAGS) $(ROOTFLAGS) -o $@ $^ $(ROOTLIBS)

$(OBJECTS): $(SOURCES)
		$(CC) $(CFLAGS) $(ROOTFLAGS) -c $<

clean:
		rm -f $(TARGET) $(OBJECTS)

