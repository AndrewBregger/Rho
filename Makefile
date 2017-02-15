CXX=g++
CFLAGS=-pedantic -Wall -Werror -Wextra -I./src
debug=0

SOURCE = $(shell find . -path "*.cpp")
OBJECTS = $(SOURCE:.cpp=.o)

EXECUTABLE=rhoc

ifeq ($(debug), 1)
	CFLAGS += -g
endif

$(EXECUTABLE) : $(OBJECTS)
	@echo "Linking"
	@$(CXX) $(CFLAGS) $(OBJECTS) -o $@

.cpp.o: $(SOURCE)
	@echo "Compiling" $<
	@$(CXX) -c $(CFLAGS) $< -o $@


clean:
	rm $(OBJECTS)
	rm $(EXECUTABLE)
