###
###	Lifegame using OpenCL
###
###	Copyright (C) 2012 K.Ohta
###

CXX = g++
CC = gcc
CFLAGS = -O0 -g
#CFLAGS = -O3 -ftree-vectorize -msse -mmmx -msse2


CLSDK_INC = /usr/include
CLSDK_LIB = -lOpenCL

.SUFFIXES: .c .cpp .asm .o
SRCS = main.cpp \
	drv_console.cpp drv_sdl.cpp
	
CLSRCS = lifegame.cl


CSRCS =	$(filter %.c, $(SRCS))
CXXSRCS = $(filter %.cpp, $(SRCS))
#CXXSRCS += $(patsubst %.cl,%.cpp,$(CLSRCS))

OBJS = $(patsubst %.c,%.o,$(CSRCS))
OBJS += $(patsubst %.cpp,%.o,$(CXXSRCS))
OBJS += $(patsubst %.cl,%.o,$(CLSRCS))

CFLAGS += -I${CLSDK_INC}
CXXFLAGS = ${CFLAGS}

.PHONY: clean 
all: lifegame

# rules
lifegame: ${OBJS}
	$(CXX) -o lifegame $^ ${CLSDK_LIB} -lSDL -lGL
	
%.o: %.cpp
	$(CXX) ${CXXFLAGS} -o $@ -c $(filter %.cpp, $<)

%.o: %.c
	$(CC) ${CFLAGS} -o $@ -c $(filter %.c, $<)

%.o: %.cl
	gawk -f cl2cpp.awk -v VARNAME=$(patsubst %.cl,%,$<) $(filter %.cl, $<) > $(patsubst %.cl,%.cpp,$<)
	$(CXX) ${CFLAGS} -o $@ -c $(patsubst %.cl,%.cpp,$<)
	rm $(patsubst %.cl,%.cpp,$<)

clean:
	rm -f *~ *.o lifegame
	
