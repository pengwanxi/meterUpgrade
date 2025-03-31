#!/bin/sh

sudo killall meterUpgrade

sleep 2

container restart esd_cco
sleep 2
container restart u_breaker
sleep 2

echo "关闭LTU升级程序,启动breaker和ccoRouter"
