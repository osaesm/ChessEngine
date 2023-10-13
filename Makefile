.PHONY: all
all: helloworld

helloworld: helloworld.cpp
	g++ -o out/helloworld helloworld.cpp

.PHONY: clean
clean:
	rm -f helloworld