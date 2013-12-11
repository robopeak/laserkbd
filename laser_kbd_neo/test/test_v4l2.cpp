/*                                                                              
 * Copyright (C) 2013 Deepin, Inc.                                                 
 *               2013 Leslie Zhai                                                  
 *                                                                              
 * Author:     Leslie Zhai <zhaixiang@linuxdeepin.com>                           
 * Maintainer: Leslie Zhai <zhaixiang@linuxdeepin.com>                           
 *                                                                              
 * This program is free software: you can redistribute it and/or modify         
 * it under the terms of the GNU General Public License as published by         
 * the Free Software Foundation, either version 3 of the License, or            
 * any later version.                                                           
 *                                                                              
 * This program is distributed in the hope that it will be useful,              
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                
 * GNU General Public License for more details.                                 
 *                                                                              
 * You should have received a copy of the GNU General Public License            
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.        
 */

#include <iostream>
#include <linux/videodev2.h>
#include <libv4l2.h>
#include <fcntl.h>

int main(int argc, char* argv[]) 
{
    std::cout << "DEBUG: " << int(O_RDWR) << std::endl;
    int fd = v4l2_open(argv[1] ? argv[1] : "/dev/video0", O_RDWR);
    struct v4l2_control ctrl;

    if (fd == -1) 
        std::cout << "fail to open video device" << std::endl;

    ctrl.id = V4L2_CID_EXPOSURE_AUTO;
    ctrl.value = V4L2_EXPOSURE_MANUAL;
    if (v4l2_ioctl(fd, VIDIOC_S_CTRL, &ctrl) == -1) 
        std::cout << "fail to set exposure auto" << std::endl;
    
    ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ctrl.value = 2000;
    if (v4l2_ioctl(fd, VIDIOC_S_CTRL, &ctrl) == -1) 
        std::cout << "fail to set exposure absolute" << std::endl;

    v4l2_close(fd);
    fd = -1;
    
    return 0;
}
