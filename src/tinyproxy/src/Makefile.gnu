CFLAGS += -DHAVE_CONFIG_H -I. -I..  -DSYSCONFDIR=\"/usr/local/etc/tinyproxy\" -DLOCALSTATEDIR=\"/usr/local/var\" -DNDEBUG -fdiagnostics-show-option -Wall

all: tinyproxy

clean:
	rm -f tinyproxy
	rm -f *.o

.c.o:
	$(CC) -I. $(CFLAGS) -c $< -o $@

tinyproxy: \
http-message.o \
html-error.o \
heap.o \
hashmap.o \
filter.o \
daemon.o \
conns.o \
connect-ports.o \
conf.o \
child.o \
buffer.o \
authors.o \
anonymous.o \
acl.o \
vector.o \
utils.o \
upstream.o \
transparent-proxy.o \
text.o \
stats.o \
sock.o \
reverse-proxy.o \
reqs.o \
network.o \
main.o \
log.o
	$(CC) $(CFLAGS)  -o $@ $^

