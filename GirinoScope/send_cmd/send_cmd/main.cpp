//
//  send_cmd.cpp
//  GirinoScope
//
//  Created by Hitchcock, Michael, E. on 4/9/14.
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

using namespace std;




int main(int argc, const char * argv[]) {
    int fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
    
    
    check_ready(fd);  // GirinoScope will report "Girino Ready"
    //get_presets(fd);  // Write command "d", and GirinoScope will report:
    // initial buffer size, baud rate, wait duration,
    // prescaler, trigger, and threshold.


    cout<<send_cmd(fd, *argv[2], 1020)<<endl;

    
}