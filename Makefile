CXX = gcc

LDLIBS = -lcurl -lpcre

SOURCES = sirius.c \
	  ask_question.c \
	  pcrs.c
TARGET = sirius

all: build

clean:
	@rm -f $(TARGET) *.o 

build: $(SOURCES)
	$(CXX) $(SOURCES) $(LDLIBS) -o $(TARGET)
