all: deps compile

compile:
	./src/compile.sh

deps:
	./utils/debian_deps.sh
