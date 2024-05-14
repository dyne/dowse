FROM dyne/devuan:excalibur
WORKDIR /opt/dowse

# from dyne/devuan
# RUN echo "Acquire::Retries \"5\";" > /etc/apt/apt.conf.d/avoid-timeouts
# RUN sed -i 's/deb.devuan/packages.devuan/' /etc/apt/sources.list

# DNSCrypt proxy
RUN apt-get -qy update && apt-get -qy upgrade && \
	apt-get install -qy --no-install-recommends make gcc g++ \
    libtool autoconf automake libltdl-dev libsodium-dev libldns-dev libjemalloc-dev \
    libhiredis-dev autoconf automake libtool git pkg-config liblo-dev \
	libmosquitto-dev redis-server mosquitto \
    && apt-get -qy clean

RUN git clone https://github.com/dyne/dnscrypt-proxy --depth 1 --branch dowse \
    && cd dnscrypt-proxy && autoreconf -i \
    && ./configure --without-systemd --disable-static --enable-plugins --disable-plugins-root \
    && make && make install && cd - && rm -rf dnscrypt-proxy

# Dowse plugin
RUN git clone https://github.com/dyne/domain-list --depth 1


CMD /bin/bash
