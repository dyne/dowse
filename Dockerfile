FROM devuan/devuan:stable
RUN apt-get -qy update && apt-get install -qy --no-install-recommends make gcc g++ libtool autoconf automake libltdl-dev libsodium-dev libldns-dev libjemalloc-dev libhiredis-dev && apt-get -qy clean
COPY . /dowse
WORKDIR /dowse/simple
RUN make build
CMD /bin/bash
