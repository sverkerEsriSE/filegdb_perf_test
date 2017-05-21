# Makefile - GeodatabaseManagement

include include/make.include
CXXFLAGS+=-W -fexceptions -D_CONSOLE $(CXXDEF) -Iinclude $(CXXOTHER)
LD=$(CXX) $(CXXFLAGS)
LDFLAGS+= -Llib
LIBS+=-lFileGDBAPI
ifndef TARGET
TARGET=bin/GeodatabaseManagement
endif
.PHONY: all
all: $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

SOURCE_FILES= \
	./main.cpp timing.cpp

HEADER_FILES= \

SRCS=$(SOURCE_FILES) $(HEADER_FILES)

OBJS=$(SOURCE_FILES:.cpp=.o)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.PHONY: clean
clean:
	-rm -f -v $(OBJS) $(TARGET)
