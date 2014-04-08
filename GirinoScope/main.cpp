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
    
    /* perform initialization NOT OpenGL/GLUT dependent,
     as we haven't created a GLUT window yet */
    init();
    
    /* initialize GLUT, let it extract command-line
     GLUT options that you may provide
     - NOTE THE '&' BEFORE argc */
    char *myargv [1];
    int myargc=1;
    myargv [0]=strdup ("GirinoScope");
    glutInit(&myargc, myargv);
    
    /* specify the display to be single
     buffered and color as RGBA values */
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    
    /* set the initial window size */
    glutInitWindowSize((int) width, (int) height);
    
    /* create the window and store the handle to it */
    wd = glutCreateWindow(myargv[0] /* title */ );
    
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
