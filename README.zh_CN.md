laserkbd 
========

RoboPeak设计的低成本激光键盘

公开发布了所有的源代码。 

注：所有的源代码遵循LGPL。

## 依赖

### Ubuntu
```
sudo apt-get install libopencv-dev libx11-dev libv4l-dev libcurl4-openssl-dev \ 
                     libkdtree++-dev libjsoncpp-dev libusb-1.0-0-dev scons
```

### ArchLinux
```
sudo pacman -Syu                                                                
sudo pacman -S opencv libx11 v4l-utils curl jsoncpp libusbx scons
```
手动安装libkdtree++
```                                                                             
git clone git://git.debian.org/git/libkdtree/libkdtree.git                      
cd libkdtree                                                                    
./autogen.sh                                                                    
./configure --prefix=/usr                                                       
make                                                                            
sudo make install                                                               
```

## 编译 
```
cd laser_kbd_neo
scons
```
* 不同的键盘布局                                                
https://github.com/xiangzhai/laserkbd/issues/2                                  
```                                                                             
vi ./laser_kbd_neo/SConstruct                                                   
feature_pattern_type_2 = 0                                                      
```

## 调试 
```
gdb ./laser_kbd_neo/laser_kbd 
r
```

如果遇到段错误
```
bt
```

例如设置断点
```
b ./laser_kbd_neo/src/port/linux/powervideocap_linux.cpp:41
```

## 视频设备是否支持曝光 
```
v4l2-ctl -w --all | grep exposure
```

## keycode
```
sudo pacman -S xorg-xev
xev 
```
敲击键盘查看keycode
