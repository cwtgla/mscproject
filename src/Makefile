# Compiler
CC = gcc

# Compiler flags
CFLAGS= -std=c99 -g -Wall

# Target build executable
TARGET = transformation

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c -O2

clean:
	$(RM) $(Target)

#transformation: transformation.c
#	gcc -o transformation.exe transformation.c -O2
