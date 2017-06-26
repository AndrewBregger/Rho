CXX=g++
CFLAGS=-pedantic -Wall -Werror -I./src
debug=1

SOURCE = $(shell find . -path "*.cpp")
OBJECTS = $(SOURCE:.cpp=.o)

EXECUTABLE=rhoc

ifeq ($(debug), 1)
	CFLAGS += -g
endif

$(EXECUTABLE) : $(OBJECTS)
	@echo "Linking Compiler"
	@$(CXX) $(CFLAGS) $(OBJECTS) -o $@

.cpp.o: $(SOURCE)
	@echo "Compiling" $<
	@$(CXX) -c $(CFLAGS) $< -o $@


clean:
	@echo "Cleaning Objects and" $(EXECUTABLE)
	@rm $(OBJECTS)
	@rm $(EXECUTABLE)
