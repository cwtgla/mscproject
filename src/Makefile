CC=gcc
LIBS=-lm
LIBS2=-lzfp

ZFPLIB=/home/crags/Documents/zfp/lib #set to lib path for local zfp installation
ZFPINC=/home/crags/Documents/zfp/include #set to include path for local zfp installation

IFLAG=-I$(ZFPINC)
LFLAG=-L$(ZFPLIB)

evaluate: compressor.o zfp_example.o
	$(CC) analysis.c compressor.o zfp_example.o $(LFLAG) $(LIBS) $(LIBS2) -o evaluate -O

test: compressor.o
	$(CC) compressor.o tests.c $(LIBS) -o test

zfp_example.o:
	$(CC) -c zfp_example.c $(IFLAG) $(LFLAG) $(LIBS2)

compressor.o: compressor.c
	$(CC) -c compressor.c

clean:
	rm -f compressor.o zfp_example.o evaluate test
