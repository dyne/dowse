all: database pgl dnscap dnscrypt-proxy src
	@echo
	@echo "Dowse is compiled and ready to run"
	@echo "setup use conf/settings and launch ./start.sh"
	@echo "or use the interactive zshell with source dowse"
	@echo


database:
	./src/database.sh

dnscrypt-proxy:
	./src/compile.sh dnscrypt-proxy

pgl:
	./src/compile.sh pgl

dnscap:
	./src/compile.sh dnscap

src:
	make -C src

clean:
	rm -rf run
	./src/compile.sh clean
	make -C src clean
	rm -f src/database.h src/database.zkv
