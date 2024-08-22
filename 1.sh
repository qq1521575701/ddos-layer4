#!/bin/bash

# 遍历当前目录下的所有 .c 文件
for file in *.c; do
    # 提取文件名（不含扩展名）
    filename=$(basename "$file" .c)
    # 编译 .c 文件生成与文件名相同的可执行文件，并链接 pthread 库
    gcc "$file" -o "$filename" -lpthread
    # 检查编译是否成功
    if [ $? -eq 0 ]; then
        echo "编译成功: $file -> $filename"
    else
        echo "编译失败: $file"
    fi
done
