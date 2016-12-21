CC = g++
CFLAGS = -O2 -Wall
INCLUDE = -I./include
MAINDIR = ./src/main
MAINS = $(wildcard $(MAINDIR)/*.cpp)
MAINOBJECTS = $(notdir $(MAINS:.cpp=.o))
TARGETS = $(basename $(MAINS))
SRCDIR = ./src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJDIR = ./obj
OBJECTS = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))

#$(TARGET): $(MAINOBJ) $(OBJECTS)
$(TARGETS): %: %.o $(OBJECTS)
	$(CC) -o $@ $^

$(MAINDIR)/%.o: $(MAINDIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGETS)
	rm -f $(OBJECTS) $(TARGET)

#参考　
#トリビアなmakefile
#http://www.jsk.t.u-tokyo.ac.jp/~k-okada/makefile/
#シンプルなmakefile
#http://urin.github.io/posts/2013/simple-makefile-for-clang/
#C++　ディレクトリ別makefile
#http://qiita.com/maaato414/items/89566fc7addc82540e4f