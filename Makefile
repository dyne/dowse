all: dnsmasq pgl dnscap

pgl:
	./src/compile.sh pgl

dnscap:
	./src/compile.sh dnscap

dnsmasq:
	./src/import.sh dnsmasq

clean:
	rm -rf run
	./src/compile.sh clean
