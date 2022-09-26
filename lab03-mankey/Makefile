main: main.c match.c
	clang -g -fsanitize=address,undefined $^ -o $@
test: main test.sh
	./test.sh
clean:
	rm -f main
fuzzer: fuzz.c match.c
	clang-fuzzer -g -fsanitize=address,undefined $^ -o $@
