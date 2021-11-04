<br/>

<a>
   <p align="center">
      <img width="40%" src=".images/azure.png">
      <img style="padding-left:10vw" width="40%" src=".images/microros_logo.png">
   </p>
</a>
<br/>

# micro-ROS app for Microsoft Azure RTOS

[![CI](https://github.com/TiejunMS/micro_ros_azure_rtos_app/actions/workflows/ci.yml/badge.svg)](https://github.com/micro-ROS/micro_ros_azure_rtos_app/actions/workflows/ci.yml)

This example application has been tested in Azure RTOS 6.1.7 and STMicroelectronics B-L475E-IOT01A.

## Usage

1. Install [Visual Studio Code](https://code.visualstudio.com/download) and [Docker engine](https://docs.docker.com/engine/install/)

2. Clone this repo. See [here](https://docs.docker.com/desktop/windows/wsl/#best-practices) for best practices on Windows:

```bash
git clone https://github.com/TiejunMS/micro_ros_azure_rtos_app
```

3. Open the micro_ros_azure_rtos_app folder in VSCode dev container by *Remote-Containers: Reopen in Container* command. See tutorial [here](https://code.visualstudio.com/docs/remote/containers-tutorial)

4. Update WIFI credentials in [app/secrets.h](app/secrets.h)

5. Update IP address of your PC in [app/main.c](app/main.c#L116)

6. Configure the CMake project

```bash
cmake -Bbuild -GNinja
```

7. Build the CMake project

```bash
cmake --build build
```

8. Run microROS agent in VSCode terminal

```bash
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
```

9. Download `build/app/stm32l475_azure_iot.bin` and copy it to USB disk shown in your PC

## Output

When the device is connected, the output from microROS terminal is,
```
[1636002041.354267] info     | UDPv4AgentLinux.cpp | init                     | running...             | port: 8888
[1636002041.355627] info     | Root.cpp           | set_verbose_level        | logger setup           | verbose_level: 4
[1636002048.937087] info     | Root.cpp           | create_client            | create                 | client_key: 0xXXXXXXXX, session_id: 0xXX
[1636002048.937165] info     | SessionManager.hpp | establish_session        | session established    | client_key: 0xXXXXXXXX, address: xx.xx.xx.xx:xxxx
```

Output from serial is,
```
Starting micro-ROS thread

Initializing WiFi
        Module: ISM43362-M3G-L44-SPI
        MAC address: xx:xx:xx:xx:xx:xx
        Firmware revision: C3.5.2.5.STM
        Connecting to SSID 'xxx'
SUCCESS: WiFi connected to xxx

Initializing DHCP
        IP address: xx.xx.xx.xx
        Gateway: xx.xx.xx.xx
SUCCESS: DHCP initialized

Initializing DNS client
        DNS address: xx.xx.xx.xx
SUCCESS: DNS client initialized

Sent: 0
Sent: 1
Sent: 2
...
```

## Purpose of the Project

This software is not ready for production use. It has neither been developed nor
tested for a specific use case. However, the license conditions of the
applicable Open Source licenses allow you to adapt the software to your needs.
Before using it in a safety relevant setting, make sure that the software
fulfills your requirements and adjust it according to any applicable safety
standards, e.g., ISO 26262.

## License

This repository is open-sourced under the Apache-2.0 license. See the [LICENSE](LICENSE) file for details.

For a list of other open-source components included in ROS 2 system_modes,
see the file [3rd-party-licenses.txt](3rd-party-licenses.txt).

## Known Issues/Limitations

There are no known limitations.
