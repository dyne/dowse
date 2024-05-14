# docker build . -t dyne/dohd
# docker run -p 8053:8053 -it dyne/dohd
# --build-arg WOLFSSL_VER=5.7.0
FROM wolfssl/wolfssl:alpine-lib as builder
ARG WOLFSSL_VER=5.7.0

RUN apk add --no-cache gcc musl-dev linux-headers make unzip wget ca-certificates \
                       git autoconf automake libtool util-linux
# build wolfssl
RUN wget -q https://github.com/wolfSSL/wolfssl/archive/refs/tags/v$WOLFSSL_VER-stable.tar.gz \
	&& tar xf v$WOLFSSL_VER-stable.tar.gz && rm -f v$WOLFSSL_VER-stable.tar.gz \
	&& cd wolfssl-$WOLFSSL_VER-stable && autoreconf -i \
	&& ./configure --enable-all && make && make install
# build dohd
RUN apk add --no-cache nghttp2-dev nghttp2-libs
RUN git clone https://github.com/dyne/dohd && make -C dohd/src \
	&& cp dohd/src/dohd /usr/bin
# make snakeoil cert
RUN apk update && apk add openssl \
    && openssl req -nodes -newkey rsa:4096 -keyout /etc/test.key -out /etc/test.csr \
    -subj "/C=IT/ST=Rome/L=Rome/O=Local Host/OU=Testing Department/CN=example.com" \
    && openssl x509 -req -sha256 -days 365 \
    -in /etc/test.csr -signkey /etc/test.key -out /etc/test.crt
# make the final image
FROM wolfssl/wolfssl:alpine-lib
ENV DOHD_PORT=443
COPY --from=builder /usr/bin/dohd /usr/bin/dohd
COPY --from=builder /usr/local/lib/libwolfssl.so* /usr/lib/
COPY --from=builder /usr/lib/libnghttp2.so* /usr/lib
COPY --from=builder /etc/test.* /etc/
RUN adduser -D -H -s /bin/false dohd
RUN chown dohd:dohd /etc/test.*
CMD echo "Listening on port $DOHD_PORT" && dohd -c /etc/test.crt -k /etc/test.key -p $DOHD_PORT -u dohd -v -F
