PROJECT = openambit_cli
CPP_FILES =	openambit_cli.cpp \
		libmovescount/log/logentry.cpp\
		libmovescount/log/logstore.cpp\
		libmovescount/movescountjson.cpp\
		libmovescount/movescountlogdirentry.cpp\
		libmovescount/movescountxml.cpp

OBJ_FILES := $(CPP_FILES:.cpp=.o)

CFLAGS  = -Wall -pedantic -std=c++11 -I./libmovescount/ -I./libmovescount/log/ -I./libs/include -L./libs/lib64
QT_LIBS = $(shell pkg-config --cflags --libs QtCore QJson ) -lz -lambit

all: $(PROJECT)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(QT_LIBS) -c $(CFLAGS) -o $@ $<

$(PROJECT): $(OBJ_FILES)
	 $(CXX) $(CXXFLAGS) $(QT_LIBS) $(CFLAGS) -o $(PROJECT) $(OBJ_FILES)

.PHONY: clean

clean:
	rm -rf $(PROJECT) *.o movescount/*.o  movescount/log/*.o build/* libs/*
