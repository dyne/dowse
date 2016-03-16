all: sources
	@echo
	@echo "Dowse is compiled and ready to run"
	@echo "setup conf/settings and launch ./start.sh"
	@echo "or use interactive zsh mode:"
	@echo "source dowse"
	@echo "dowse-start"
	@echo "dowse-[tab] for completion of other commands"
	@echo


sources:
	make -C src

clean:
	make -C src clean

#	rm -f src/database.h src/database.zkv
#	rm -rf run
