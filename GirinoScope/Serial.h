//
//  Serial.h
//  GirinoScope
//
//  Created by Hitchcock, Michael, E. on 4/8/14.
//  Copyright (c) 2014 Hitch. All rights reserved.
//

#ifndef GirinoScope_Serial_h
#define GirinoScope_Serial_h

char *inFromArduino = new char[255];
char *outToArduino = new char[255];


// Get Presets Function
void get_presets(int fd) {
    
    tcflush(fd, TCIFLUSH);

    int i=0;
    while (i<=5 && !write(fd, "d", 1)) i++;

    int bytes = 0;
    while (bytes < 109) {
        ioctl(fd, FIONREAD, &bytes);
    }

    read(fd,inFromArduino,109);
    
//    char* presets = strtok(inFromArduino, ":");
//    char* presets1 = strtok(presets, ":");
    
    
//    std::cout<<presets[2]<<std::endl;
    std::cout<<inFromArduino<<std::endl<<std::flush;
}

#endif
