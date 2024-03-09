# Compiler
CC = g++
# Compiler flags
CFLAGS = -std=c++17 -Wall
# Source files
SRCS =  main.cpp	\
		aida.cpp	\
		serial.cpp
# Executable name
TARGET = esp_aida.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
