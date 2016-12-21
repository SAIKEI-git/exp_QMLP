CC = g++
CFLAGS = -O2 -Wall
INCLUDE = -I./include
MAINDIR = ./src/main
MAIN = qmlp_test
MAINOBJ = $(MAINDIR)/$(MAIN).o
TARGET = ./bin/$(MAIN)
OBJDIR = ./obj
SRCDIR = ./src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))

$(TARGET): $(MAINOBJ) $(OBJECTS)
	$(CC) -o $@ $^

$(MAINDIR)/$(MAIN).o: $(MAINDIR)/$(MAIN).cpp
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)
	rm -f $(OBJECTS) $(TARGET)