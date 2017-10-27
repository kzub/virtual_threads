CC := clang++
CFLAGS := -g -O3 -Wall -Werror -std=c++0x
INC := -I src

bin/example: build/virtual_threads.o build/example.o
	$(CC) $^ -o bin/example

build/virtual_threads.o: src/virtual_threads.cpp src/virtual_threads.h
	mkdir -p build
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

build/example.o: example/test.cpp build/virtual_threads.o
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

run:
	bin/example

clean:
	@echo "Cleaning...";
	rm -r build bin/example
	

