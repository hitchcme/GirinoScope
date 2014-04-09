//
//  Serial.h
//  GirinoScope
//
//  Created by Hitchcock, Michael, E. on 4/8/14.
//  Copyright (c) 2014 Hitch. All rights reserved.
//

#ifndef GirinoScope_Serial_h
#define GirinoScope_Serial_h

int fd;

char *inFromArduino = new char[255];
char *outToArduino = new char[255];
int inFromArduino1[1020];


long int buffsiz;
long int baudrat;
long int waitdur;
long int prescal;
long int trigger;
long int thresho;

void check_ready(int fd) {
    int Scope_Not_Ready = 1;
    
    
    while (Scope_Not_Ready == 1) {
        
        if (fd == -1) {
            std::cout << "Error Opening Port" << std::endl;
            Scope_Not_Ready=0;
            close(fd);
            exit(0);
        }
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
            std::cout<<"Scope Not Ready"<<std::endl;
            close(fd);
            sleep(1);
        }
    }
}

// Get Presets Function
void get_presets(int fd) {
    
    tcflush(fd, TCIFLUSH);

    while (!write(fd, "d", 1));

    int bytes = 0;
    
    while (bytes < 109) {
        ioctl(fd, FIONREAD, &bytes);
    }

    read(fd,inFromArduino,109);
    
    char* presets_ch = strtok(inFromArduino, ":\n");

        presets_ch = strtok(NULL, ":\n");
    buffsiz=atol(presets_ch);
        presets_ch = strtok(NULL, ":\n");
        presets_ch = strtok(NULL, ":\n");
    baudrat=atol(presets_ch);
        presets_ch = strtok(NULL, ":\n");
        presets_ch = strtok(NULL, ":\n");
    waitdur=atol(presets_ch);
        presets_ch = strtok(NULL, ":\n");
        presets_ch = strtok(NULL, ":\n");
    prescal=atol(presets_ch);
        presets_ch = strtok(NULL, ":\n");
        presets_ch = strtok(NULL, ":\n");
    trigger=atol(presets_ch);
        presets_ch = strtok(NULL, ":\n");
        presets_ch = strtok(NULL, ":\n");
    thresho=atol(presets_ch);

}

std::string send_cmd(int fd, char cmd, int bytes) {

    int byte_cnt = 0;
    int bytes_at_port = 0;
    
    int i1=0;
    while (i1 <= 5 && byte_cnt <= 0) {
        i1++;
        tcflush(fd, TCIFLUSH);
        tcflush(fd, TCOFLUSH);
        
        std::cout<<"Main Loop Iteration: "<<i1<<std::endl<<std::flush;
        
        byte_cnt = 0;
        bytes_at_port = 0;

        int i21=0;
        while (i21 < 5 && write(fd, &cmd, 1) <= 0) {
            std::cout<<"While Loop 1 Iteration:"<<i21<<std::endl<<std::flush;
            i21++;
        }
        int i22 = 0;
        while (i22 < 100000 && bytes_at_port < bytes) {
            ioctl(fd, FIONREAD, &bytes_at_port);
            std::cout<<"While Loop 2 Iteration:"<<i22<<"   BAP: "<<bytes_at_port<<"  BTWF: "<<bytes<<std::endl<<std::flush;
            i22++;
        }
        
        int i23 = 0;
        while (i23 < 5 && byte_cnt < bytes_at_port) {
            byte_cnt += read(fd,inFromArduino,bytes);
            std::cout<<"While Loop 3 Iteration:"<<i23<<std::endl<<std::flush;
            i23++;
        }

        sleep(1);
        
    }
    
    return inFromArduino;
}

#endif
