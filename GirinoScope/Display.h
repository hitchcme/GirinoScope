//
//  Display.h
//  GirinoScope
//
//  Created by Hitchcock, Michael, E. on 4/8/14.
//  Copyright (c) 2014 Hitch. All rights reserved.
//

#ifndef GirinoScope_Display_h
#define GirinoScope_Display_h



#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#define NENDS 2           /* number of end "points" to draw */

GLdouble width, height;   /* window width and height */
int wd;                   /* GLUT window handle */
int ends[NENDS][4];       /* array of 2D points */

char *myargv [1];
int myargc=1;




/* Program initialization NOT OpenGL/GLUT dependent,
 as we haven't created a GLUT window yet */
void init_disp(void) {
    width  = 640.0;                 /* initial window width and height, */
    height = 400.0;                  /* within which we draw. */
    ends[0][0] = (int)(0.25*width);  /* (0,0) is the lower left corner */
    ends[0][1] = (int)(0.75*height);
    ends[1][0] = (int)(0.75*width);
    ends[1][1] = (int)(0.25*height);
    
    /* initialize GLUT, let it extract command-line
     GLUT options that you may provide
     - NOTE THE '&' BEFORE argc */
    myargv [0]=strdup ("GirinoScope");
    glutInit(&myargc, myargv);
    
    /* specify the display to be single
     buffered and color as RGBA values */
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    
    /* set the initial window size */
    glutInitWindowSize((int) width, (int) height);
    
    /* create the window and store the handle to it */
    wd = glutCreateWindow(myargv[0] /* title */ );

}

/* Callback functions for GLUT */

/* Draw the window - this is where all the GL actions are */
void display(void) {
    int i;
    
    /* clear the screen to white */
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    /* draw a black line */
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(3.0);
    glBegin(GL_LINES);

//    for (i = 0; i < NENDS; i++) {
        glVertex2i(0, 0);
        glVertex2i(1, 1);
        glVertex2i(2, 4);
        glVertex2i(3, 9);
        glVertex2i(10, 100);
        glVertex2i(20, 1);
        glVertex2i(30, 4);
        glVertex2i(40, 9);

    
    tcflush(fd, TCIFLUSH);
    while (!write(fd, "s", 1));
    int bytes = 0;
    while (bytes < 1020) {
        ioctl(fd, FIONREAD, &bytes);
        }
    read(fd,inFromArduino,1020);
    std::cout<<inFromArduino<<std::endl<<std::flush;

    
//        glVertex2i(i, i1);
//        glVertex2i(i, i1*=i);
//    }
    
    glEnd();
    glFlush();
}

/* Called when window is resized,
 also when window is first created,
 before the first call to display(). */

void reshape(int w, int h) {
    /* save new screen dimensions */
    width = (GLdouble) w;
    height = (GLdouble) h;
    
    /* tell OpenGL to use the whole window for drawing */
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    
    /* do an orthographic parallel projection with the coordinate
     system set to first quadrant, limited by screen/window size */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, width, 0.0, height);
}






#endif
