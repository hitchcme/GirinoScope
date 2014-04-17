//
//  MyOpenGLView.h
//  GirinoScope
//
//  Created by Hitchcock, Michael, E. on 4/12/14.
//  Copyright (c) 2014 Hitch. All rights reserved.
//

#import <Cocoa/Cocoa.h>

// import OpenGL Stuff
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>

#include <math.h>

// import IOKit headers
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>
#include <IOKit/serial/ioss.h>
#include <sys/ioctl.h>

typedef struct {
    GLfloat x;
    GLfloat y;
} points;




@interface MyOpenGLView : NSOpenGLView {

// GirinoScope Command/Control Feedback..... kind of....
    IBOutlet NSPopUpButton *vrefListPullDown;
    IBOutlet NSSlider *prescale_Slider;
    IBOutlet NSSlider *threshold_Slider;
    IBOutlet NSMatrix *trigger_radios;
    
	IBOutlet NSPopUpButton *serialListPullDown;
	IBOutlet NSTextView *serialOutputArea;
	IBOutlet NSTextField *serialInputField;
	IBOutlet NSTextField *baudInputField;
	int serialFileDescriptor; // file handle to the serial port
	struct termios gOriginalTTYAttrs; // Hold the original termios attributes so we can reset them on quit ( best practice )
	bool readThreadRunning;
	NSTextStorage *storage;
    
    IBOutlet MyOpenGLView *openGLView;
    
    
}


- (IBAction) drawRect: (NSRect) graph;

- (NSString *) openSerialPort: (NSString *)serialPortFile baud: (speed_t)baudRate;
- (void)appendToIncomingText: (id) text;

- (void)incomingTextUpdateThread: (NSThread *) parentThread;
- (void) refreshSerialList: (NSString *) selectedText;
- (void) writeString: (NSString *) str;
- (void) writeByte: (uint8_t *) val;
- (IBAction) serialPortSelected: (id) cntrl;

- (IBAction) baudAction: (id) cntrl;
- (IBAction) refreshAction: (id) cntrl;
- (IBAction) sendText: (id) cntrl;
- (IBAction) sliderChange: (NSSlider *) sldr;



- (IBAction) hitAButton: (NSButton *) btn;
- (IBAction) hitBButton: (NSButton *) btn;
- (IBAction) hitCButton: (NSButton *) btn;
- (IBAction) resetButton: (NSButton *) btn;



// GirinoScope Command/Control
- (IBAction) vrefSelected: (id) cntrl;
- (IBAction) prescaler_cmd: (NSSlider *) sldr;
- (IBAction) threshold_cmd: (NSSlider *) sldr;
- (IBAction) trigger_cmd: (NSMatrix *) tag;
- (IBAction) set_to_75_P: (NSButton *) cntrl;
- (IBAction) set_to_fifty_P: (NSButton *) cntrl;
- (IBAction) set_to_25_P: (NSButton *) cntrl;

@end
