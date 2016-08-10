TARGETS = $(BIN1)

BIN1 = wangxx
BIN1_OBJS = $(OBJS)
BIN1_LIBS = pthread log4cplus
BIN1_LFLAGS = -L$(LOG4CPLUS_ROOT)/lib
INCS = -I../src -I$(LOG4CPLUS_ROOT)/include
SOURCE := $(wildcard ../src/*.cpp)

OBJS := $(patsubst %.cpp,%.o,$(SOURCE))

CC := g++
CFLAGS := -std=c++11 -g -Wall -O3 -Wno-deprecated -fPIC -D _FILE_OFFSET_BITS=64 $(INCS) #-m64
CXXFLAGS := $(CFLAGS)
RM := rm -f

.PHONY : all objs clean rebuild

all : $(TARGETS)

objs : $(OBJS)

clean :
	$(RM) ../src/*.o
	$(RM) ../src/*.d
	$(RM) ../src/*~
	$(RM) ../bin/$(BIN1)

rebuild: clean all

$(BIN1) : $(BIN1_OBJS)
	$(CC) $(CFLAGS) -o ../bin/$@ $(BIN1_LFLAGS) $^ $(addprefix -l,$(BIN1_LIBS))

dist:
	mkdir -p .t/wangxx
	cp -a ../data .t/wangxx
	cp -a ../log .t/wangxx
	cp -a ../cfg .t/wangxx
	cp -a ../bin .t/wangxx
	cp -a ${LOG4CPLUS_ROOT}/lib/liblog4cplus*.so.*	.t/wangxx/bin/
	tar -cvf wangxx_install.tar  .t/*
	rm -rf .t
	tar -cvf wangxx_install_pkg.tar  wangxx_install.tar install.sh
	rm -f wangxx_install.tar

