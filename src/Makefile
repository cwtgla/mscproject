# Compiler
#CC = gcc

# Compiler flags
#CFLAGS= -std=c99 -g -Wall

# Target build executable
#TARGET = transformation

#all: $(TARGET)

#$(TARGET): $(TARGET).c
#	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c -O2

#clean:
#	$(RM) $(Target)

#transformation: transformation.c
#	gcc -o transformation.exe transformation.c -O2

#gcc tests.c compressor.o -lm -o tests.exe
#

#gcc compressor.c complete_compressor.o zfp_example.o -L/home/crags/Documents/zfp/lib -lzfp -lm -o run.exe

 # 928  gcc -c -I/home/crags/Documents/zfp/include -L/home/crags/Documents/zfp/lib -lzfp zfp_example.c


##todo set up -I and -L
CC=gcc
LIBS=-lm
LIBS2=-lzfp

ZFPLIB=/home/crags/Documents/zfp/lib
ZFPINC=/home/crags/Documents/zfp/include

IFLAG=-I$(ZFPINC)
LFLAG=-L$(ZFPLIB)

#obj = $(src:.c=.o)

evaluation: compressor.o zfp_example.o
	$(CC) analysis.c compressor.o zfp_example.o $(LFLAG) $(LIBS) $(LIBS2) -o evaluation

tests: compressor.o
	$(CC) compressor.o tests.c $(LIBS) -o tests

zfp_example.o:
	$(CC) -c zfp_example.c $(IFLAG) $(LFLAG) $(LIBS2)

compressor.o: compressor.c
	$(CC) -c compressor.c

clean:
	rm -f compressor.o zfp_example.o evaluation tests