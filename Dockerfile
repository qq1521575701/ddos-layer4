FROM ubuntu:22.04

RUN apt-get update -y

RUN apt-get install -y gcc

COPY . /root/

RUN gcc /root/dns.c -o /root/dns -pthread \
    && gcc /root/dvr.c -o /root/dvr -pthread \
    && gcc /root/meme.c -o /root/meme -pthread \
    && gcc /root/ntp.c -o /root/ntp -pthread \
    && gcc /root/qotd.c -o /root/qotd -pthread \
    && gcc /root/tcp.c -o /root/tcp -pthread \
    && gcc /root/wsd.c -o /root/wsd -pthread \
    && gcc /root/syn.c -o /root/syn -pthread \
    && rm -f /root/*.c

WORKDIR /root
