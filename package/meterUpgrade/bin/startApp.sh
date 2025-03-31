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
  echo "进程已终止，重新启动：$PROCESS_NAME"
fi

# 检查输入的参数数量是否足够
if [ $# -lt 2 ]; then
  echo "错误: 参数不足"
  echo "用法: $0  <升级文件(.bin)>  <程序版本号>"
  exit 1
fi

# 获取第二个和第三个参数
param2=$1
param3=$2

executable_path="/home/sysadm/meterUpgrade/bin"

cd "$executable_path" || exit

export LD_LIBRARY_PATH=/home/sysadm/meterUpgrade/lib:$LD_LIBRARY_PATH
chmod +x meterUpgrade

# 启动程序并传递参数
echo "启动程序: ./meterUpgrade $param2 $param3"
#sudo nohup ./meterUpgrade "$param2" "$param3" > /dev/null 2>&1 &

sudo LD_LIBRARY_PATH=/home/sysadm/meterUpgrade/lib nohup /home/sysadm/meterUpgrade/bin/meterUpgrade "$param2" "$param3" >/dev/null 2>&1 &
# 检查程序是否成功启动
if [ $? -ne 0 ]; then
  echo "错误: 程序启动失败"
  exit 1
else
  echo "程序已成功启动"
fi


