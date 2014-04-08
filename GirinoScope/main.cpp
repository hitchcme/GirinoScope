//
//  main.cpp
//  GirinoScope
//
//  Created by Hitchcock, Michael, E. on 4/7/14.
//  Copyright (c) 2014 Hitch. All rights reserved.
//

#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "Serial.h"

using namespace std;

char *inFromArduino = new char[255];
char *outToArduino = new char[255];

int main(int argc, const char * argv[]) {
    
    
    int fd;
    int Scope_Not_Ready = 1;
    
    while (Scope_Not_Ready == 1) {
        
        fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
        
        if (fd == -1)
            std::cout << "Error Opening Port" << std::endl;
        else
            std::cout << "fd: " << fd << std::endl;
        
        
        struct termios options;
        tcgetattr(fd,&options);
        cfsetispeed(&options,B115200);
        cfsetospeed(&options,B115200);
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        tcsetattr(fd,TCSANOW,&options);
        
        
        std::string Scope_Ready = "";
        std::string Scope_Ready1 = "Girino ready";
        int i=0;
        
        while (Scope_Ready!=Scope_Ready1 || i<=13) {
            read(fd, inFromArduino, 1);
            Scope_Ready.append(inFromArduino);
            //sleep(1);
            i++;
        }
        
        if (Scope_Ready == Scope_Ready1) {
            std::cout<<Scope_Ready<<std::endl;
            Scope_Not_Ready = 0;
        }
        else if (Scope_Ready != Scope_Ready1) {
            Scope_Not_Ready = 1;
            Scope_Ready = "";
            cout<<"Scope Not Ready"<<endl;
            close(fd);
            sleep(1);
        }
    }
    tcflush(fd, TCIFLUSH);
    write(fd, "d", 1);
    read(fd,inFromArduino,100);
    cout<<inFromArduino<<endl;
    
    cout << "program terminated" << endl;
    close(fd);
    
    return 0;
}
