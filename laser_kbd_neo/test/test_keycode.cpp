/*                                                                              
 * Copyright (C) 2013 Deepin, Inc.                                              
 *               2013 Leslie Zhai <zhaixiang@linuxdeepin.com> 
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
#include <X11/Xlib.h>

static Display  *   m_display   = NULL;
static Visual   *   m_visual    = NULL;
static Window       m_window;
static Atom         wmDeleteMessage;
static bool         m_isLooping = true;
static int          m_width     = 400;
static int          m_height    = 300;

static void m_cleanup();
static void m_loop();

int main(int argc, char* argv[]) {
    Window rootWin;
    int screen;
                                                                                
    if (!(m_display = XOpenDisplay(NULL))) { 
        printf("ERROR: fail to XOpenDisplay\n");
        return 0;
    }

    screen = DefaultScreen(m_display);                                                     
    rootWin = RootWindow(m_display, screen);                                               
    
    // The actual window, front buffer, using Xdbe enabled Visual
    unsigned long xAttrMask = CWBackPixel;
    XSetWindowAttributes xAttr;
    xAttr.background_pixel = BlackPixel(m_display, screen);
    m_window = XCreateWindow(m_display, rootWin, 0, 0, m_width, m_height, 0, 
                             CopyFromParent, CopyFromParent, m_visual, 
                             xAttrMask, &xAttr);
    if (!m_window) {
        printf("ERROR: fail to XCreateWindow\n");
        return 0;
    }

    XStoreName(m_display, m_window, "TestKeyCode");
    XSelectInput(m_display, 
                 m_window, 
                 ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(m_display, m_window);

    wmDeleteMessage = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_display, m_window, &wmDeleteMessage, 1);

    m_loop();

    m_cleanup();

    return 0;
}

static void m_cleanup() 
{
    if (m_display) {
        XDestroyWindow(m_display, m_window);
        XCloseDisplay(m_display);
        m_display = NULL;
    }
}

static void m_loop() 
{
    XEvent e;

    m_isLooping = true;
    while (m_isLooping) {
		XNextEvent(m_display, &e);
        switch (e.type) {
        case Expose:
            break;
        case ClientMessage: 
            if (e.xclient.data.l[0] == wmDeleteMessage) {
                m_isLooping = false;
                break;
            }
            break;
        case KeyPress:
            XKeyEvent xkey = e.xkey;
            printf("DEBUG: 0x%04x %u -> %u\n", 
                   XKeycodeToKeysym(m_display, xkey.keycode, 0), 
                   XKeycodeToKeysym(m_display, xkey.keycode, 0), 
                   xkey.keycode);
            switch (XKeycodeToKeysym(m_display, xkey.keycode, 0)) {
            case 'q':
                m_isLooping = false;
                break;
            }
        }
	}
}
