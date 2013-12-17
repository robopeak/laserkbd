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

#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

void click_key(Display* display , KeyCode keycode) 
{
    XTestFakeKeyEvent(display, keycode, True, 0); // key press event
    XTestFakeKeyEvent(display, keycode, False, 0); // key release event
    XFlush(display);

    return;
}

int main(int argc, char* argv[]) 
{
    Display* display = XOpenDisplay(NULL);

    printf("DEBUG: %d\n", XKeysymToKeycode(display, 10));
    // xmodmap -pke
    click_key(display, 56);

    XCloseDisplay(display);
    display = NULL;

    return 0;
}
