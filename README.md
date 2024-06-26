### 安装 （docker）
apt update -y && apt install docker.io -y && git clone https://github.com/qq1521575701/dd.git && cd dd && docker build -t ddos . && docker run -itd --name ddos --network host ddos:latest && docker exec -it ddos /bin/bash
