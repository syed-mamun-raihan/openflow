EXECUTABLE = cofa
CC = g++
#-fno-rtti must be used with main executable - test app may needs different flagset
CXXFLAGS = -c -ggdb -Wall -Werror -std=gnu++11 -pthread -fvisibility=hidden -fvisibility-inlines-hidden
DEFINES = -DSTANDALONE -DDEBUG -D_GLIBCXX_USE_NANOSLEEP
LDFLAGS = -lssl -lcrypto -lpthread -ldl -lrt -llinx
LFLAGS = -Llibs
INC = ../jsoncpp\
      ../mongoosecpp/src\
	  ./

# This foreach loop puts a -I in front of each include dir, keeping the makefile clean
INC_PARAM:=$(foreach dir, $(INC), -I$(dir))

SRC= ../jsoncpp/jsoncpp.cpp\
	 $(wildcard ../mongoosecpp/src/*.cpp)\
	 $(wildcard ./*.cpp)

###
###			GMock / GTest Variables
###
OBJECTS:=$(SRC:.cpp=.o)


###
###			Targets
###
all: $(EXECUTABLE)

#-fno-rtti must be used with main executable - test app may needs different flagset
release: CXXFLAGS = -c -O2 -Wall -std=gnu++11 -pthread -flto -fvisibility=hidden -fvisibility-inlines-hidden
release: $(EXECUTABLE)


$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CXXFLAGS) $(INC_PARAM) $(DEFINES) $< -o $@

docs:
	doxygen Doxyfile

#tests:
.PHONY: clean
clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)

