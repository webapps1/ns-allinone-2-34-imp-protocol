CC=g++
CPPFLAGS=-Wall -DDEBUG

BIN=vanets

all: main
	mv main $(BIN)

main: main.o vanets_queue.o vanets_buffer_msgs.o

clean:
	find $(PWD) -type f -name "*.o" -exec rm -rf {} \;
distclean:
	find $(PWD) -type f -name "*.o" -exec rm -rf {} \;
	find $(PWD) -type f -name "*~" -exec rm -rf {} \;
	rm -rf $(PWD)/$(BIN)
