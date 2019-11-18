#!/bin/sh

apt-get -y -q update
apt-get -y -q --no-install-recommends install \
		autoconf \
		automake \
		build-essential \
		cmake \
		hostapd \
		ipset \
		iptables \
		isc-dhcp-server \
		iw \
		kmod \
		libcap2-bin \
		libhiredis-dev \
		libhiredis-dev \
		libjemalloc-dev \
		libkmod-dev \
		libldns-dev \
		liblo-dev \
		libltdl-dev \
		libmosquitto-dev \
		libnetfilter-queue-dev \
		libsodium-dev \
		libtool \
		libwebsockets8 \
		mosquitto \
		netdata \
		nmap \
		pkg-config \
		python3-flask \
		python3-redis \
		python3-tldextract \
		redis \
		snooze \
		supervisor \
		uuid-dev \
		wget \
		xmlstarlet \
		zlib1g-dev \
		zsh

