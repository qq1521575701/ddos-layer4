FROM ubuntu:22.04

RUN apt-get update -y

RUN apt-get install -y gcc

COPY . /root/

RUN gcc dns.c -o dns -pthread \
    && gcc dvr.c -o dvr -pthread \
    && gcc meme.c -o meme -pthread \
    && gcc ntp.c -o ntp -pthread \
    && gcc qotd.c -o qotd -pthread \
    && gcc tcp.c -o tcp -pthread \
    && gcc wsd.c -o wsd -pthread \
    && rm -f *.c

WORKDIR /root
