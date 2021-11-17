#!/bin/bash

. "/opt/ros/foxy/setup.sh"
. "/uros_ws/install/local_setup.sh"

if [ $# -lt 1 ]; then
    echo "Usage: $0 \"<connection string>\""
    exit 1
fi

/opt/ros/foxy/bin/ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888 > /tmp/microros_agent.log &

cd $(dirname $0)/service
npm install
npm run build

echo "Start microROS agent proxy"
node ./lib/microros_agent_proxy.js --connectionString="$1" --port 8888 --address "127.0.0.1"