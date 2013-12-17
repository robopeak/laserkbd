laserkbd 
========

RoboPeak设计的低成本激光键盘

公开发布了所有的源代码。 

注：所有的源代码遵循LGPL。

下载Linux Deepin 
================
http://www.linuxdeepin.com/download.cn.html 

依赖项 
======
```
sudo apt-get install libopencv-dev libx11-dev libv4l-dev libcurl4-openssl-dev \
                     libkdtree++-dev libjsoncpp-dev libusb-1.0-0-dev scons 
```

编译 
====
```
cd laser_kbd_neo
scons
```

调试 
====
```
gdb ./laser_kbd_neo/laser_kbd 
r
```

```
bt
```

```
b ./laser_kbd_neo/src/port/linux/powervideocap_linux.cpp:41
```

视频设备是否支持曝光 
====================
```
sudo apt-get install v4l-utils
v4l2-ctl -w --all | grep exposure
```

opencv 2.4.2 BUG 
================
BUG: OpenCV Error: Null pointer (NULL guiReceiver (please create a window)) in cvDestroyWindow, file /tmp/buildd/opencv-2.4.2+dfsg/modules/highgui/src/window_QT.cpp, line 489

```
apt-get source libopencv-dev
vi modules/highgui/src/window_QT.cpp
:489
return;
```
