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

    check_ready(fd);            //  Check Girino Ready.
    
    cout<<inFromArduino<<endl;
    
    send_cmd(fd, "r3", 2, 109);
    
    cout<<inFromArduino<<endl;
    
    get_presets(fd);
        cout<<"Buffer Size:   "<<buffsiz<<endl;
        cout<<"Baud Rate:     "<<baudrat<<endl;
        cout<<"Wait Duration: "<<waitdur<<endl;
        cout<<"Prescaler:     "<<prescal<<endl;
        cout<<"Trigger:       "<<trigger<<endl;
        cout<<"Threshold:     "<<thresho<<endl;
    
//    for (int i=0; i<=20; i++) {
    send_cmd(fd, "s", 1, 1020);
        cout<<inFromArduino<<endl;
//    }
//    while (wfsd[i]) {
//        inFromArduino1[i]= static_cast<unsigned char>(inFromArduino[i]);
//        std::cout<<inFromArduino1[i]<<std::endl<<std::flush;
//    }
    
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
