#!/bin/sh

# 配置进程的关键字或名称（唯一标识）
PROCESS_NAME="meterUpgrade"

# 查找进程
PID=$(ps -ef | grep "$PROCESS_NAME" | grep -v "grep" | awk '{print $2}')

if [ -z "$PID" ]; then
    echo "$PROCESS_NAME 进程不存在"
else
    echo "进程存在，PID: $PID，正在终止..."
    kill -9 $PID
    echo "进程已终止"
fi
sleep 2

container restart esd_cco
echo "重启ccoRouter"
sleep 2
container restart u_breaker
echo "重启breaker"
sleep 2
