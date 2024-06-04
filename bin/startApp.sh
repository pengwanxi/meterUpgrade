#!/bin/sh

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
sudo nohup ./meterUpgrade "$param2" "$param3" > /dev/null 2>&1 &

# 检查程序是否成功启动
if [ $? -ne 0 ]; then
  echo "错误: 程序启动失败"
  exit 1
else
  echo "程序已成功启动"
fi

