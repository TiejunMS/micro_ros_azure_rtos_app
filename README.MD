## Introduction
This is an application to bridge microROS and microROS agent through Azure IoTHub communication.

## Usage

1. Install [Visual Studio Code](https://code.visualstudio.com/download) and [Docker engine](https://docs.docker.com/engine/install/)

2. Clone this repo and switch to *github* branch. See [here](https://docs.docker.com/desktop/windows/wsl/#best-practices) for best practices on Windows:

```bash
git clone https://github.com/TiejunMS/micro_ros_azure_rtos_app
cd micro_ros_azure_rtos_app
git checkout iothub
```

3. Open the micro_ros_azure_rtos_app folder in VSCode dev container by *Remote-Containers: Reopen in Container* command. See tutorial [here](https://code.visualstudio.com/docs/remote/containers-tutorial)

4. Run microROS agent in VSCode terminal

```bash
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
```

5. Open another terminal and install npm packages

```bash 
cd service
npm install
```

6. Set connection string to IoTHub

Edit *service/package.json* and update connection string in "test". For example,

```json
"test": "node ./lib/microros_agent_proxy.js --connectionString=\"HostName=xxx.azure-devices.net;SharedAccessKeyName=service;SharedAccessKey=xxx=\" --port 8888 --address \"127.0.0.1\""
```

7. Build and run application

```bash
npm run build
npm run test
```