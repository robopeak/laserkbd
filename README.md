laserkbd 
========

A Low Cost Laser Projection Keyboard designed by RoboPeak 

Full Source Code Release 

NOTE: all the source code is licensed under LGPL. 
No warranty for the sourcecode and the related software.

## dependence for ArchLinux 
```
sudo pacman -Syu
sudo pacman -S opencv libx11 v4l-utils curl jsoncpp libusbx scons 
manual install libkdtree++
```

## build
```
cd laser_kbd_neo
scons
```
* different keyboard layout
https://github.com/xiangzhai/laserkbd/issues/2
```
vi ./laser_kbd_neo/SConstruct 
feature_pattern_type_2 = 0
```

## debug 
```
gdb ./laser_kbd_neo/laser_kbd 
r
```

if segfault
```
bt
```

set breakpoint, for example:
```
b ./laser_kbd_neo/src/port/linux/powervideocap_linux.cpp:41
```

## video device whether or not support exposure 
```
v4l2-ctl -w --all | grep exposure
```

## keycode                                                                      
```                                                                             
sudo pacman -S xorg-xev                                                         
xev                     
```                                                        
hip keyboard too see the keycode                                                             
