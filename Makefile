build: clean
	mkdir -p build
	gcc src/main.c src/algorithms.c -lSDL2 -o build/main
.PHONY: run

run: build
	./build/main
.PHONY: clean

clean:
	rm -rf build

