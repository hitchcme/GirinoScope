//
//  main.cpp
//  GirinoScope
//
//  Created by Hitchcock, Michael, E. on 4/7/14.
//  Copyright (c) 2014 Hitch. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "Serial.h"
#include "Display.h"


//#if defined(__APPLE__)
#include <GLUT/glut.h>
//#else
//#include <GL/glut.h>
//#endif

using namespace std;




int main(int argc, const char * argv[]) {

    int fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);

    check_ready(fd);  // GirinoScope will report "Girino Ready"
    get_presets(fd);  // Write command "d", and GirinoScope will report:
                      // initial buffer size, baud rate, wait duration,
                      // prescaler, trigger, and threshold.
    get_data(fd);
    
    init_disp();
    /* --- register callbacks with GLUT --- */
    
    /* register function to handle window resizes */
    glutReshapeFunc(reshape);
    
    /* register function that draws in the window */
    glutDisplayFunc(display);
    
    /* start the GLUT main loop */
    glutMainLoop();
    
    
    
    
    cout << "program terminated" << endl;
    close(fd);

    return 0;
}
