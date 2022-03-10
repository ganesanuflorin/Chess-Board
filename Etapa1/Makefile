
CC = g++
CFLAGS = -c -Wall -Wextra
LDFLAGS = -O3
SOURCES = main.cpp
EXECUTABLE = chess
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all
all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(EXECUTABLE) $(OBJECTS) 

%.o: %.cpp %.h
	$(CC) $(CFLAGS) $(<) -o $(@)

.PHONY: clean
clean:
	rm -f *.o $(EXECUTABLE)

.PHONY: run
run: all
	./$(EXECUTABLE)

.PHONY: run-engine
run-engine: all
	xboard -fcp ./$(EXECUTABLE)
