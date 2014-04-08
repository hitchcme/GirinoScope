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
long int * get_presets(int fd) {
    
    tcflush(fd, TCIFLUSH);

    int i=0;
    while (i<=5 && !write(fd, "d", 1)) i++;

    int bytes = 0;
    while (bytes < 109) {
        ioctl(fd, FIONREAD, &bytes);
    }

    read(fd,inFromArduino,109);
    
    char* presets_ch = strtok(inFromArduino, ":\n");

        presets_ch = strtok(NULL, ":\n");
    long int buffsiz=atol(presets_ch);
        presets_ch = strtok(NULL, ":\n");
        presets_ch = strtok(NULL, ":\n");
    long int baudrat=atol(presets_ch);
        presets_ch = strtok(NULL, ":\n");
        presets_ch = strtok(NULL, ":\n");
    long int waitdur=atol(presets_ch);
        presets_ch = strtok(NULL, ":\n");
        presets_ch = strtok(NULL, ":\n");
    long int prescal=atol(presets_ch);
        presets_ch = strtok(NULL, ":\n");
        presets_ch = strtok(NULL, ":\n");
    long int trigger=atol(presets_ch);
        presets_ch = strtok(NULL, ":\n");
        presets_ch = strtok(NULL, ":\n");
    long int thresho=atol(presets_ch);

    long int presets[6];
    presets[0]=buffsiz;
    presets[1]=baudrat;
    presets[2]=waitdur;
    presets[3]=prescal;
    presets[4]=trigger;
    presets[5]=thresho;
    
    return presets;
    
//    std::cout<<"buffersize: "<<buffsiz<<std::endl;
//    std::cout<<"baudrate:   "<<baudrat<<std::endl;
//    std::cout<<"waitdur:    "<<waitdur<<std::endl;
//    std::cout<<"Prescaler   "<<prescal<<std::endl;
//    std::cout<<"Trigger     "<<trigger<<std::endl;
//    std::cout<<"Threshold   "<<thresho<<std::endl;


//    std::cout<<presets1[1,2]<<std::endl;
//    std::cout<<inFromArduino<<std::endl<<std::flush;
}

#endif
