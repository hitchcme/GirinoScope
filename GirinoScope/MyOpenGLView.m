//
//  MyOpenGLView.m
//  Golden Triangle
//
//  Created by Hitchcock, Michael, E. on 4/12/14.
//  Copyright (c) 2014 Hitch. All rights reserved.
//

#import "MyOpenGLView.h"

bool Girino_Ready=FALSE;
const int BUFFER_SIZE=1020;
points graph[BUFFER_SIZE];
int i1=0;
float height;
float width;

long int wf_Current_MAX;
long int wf_Current_MIN;


@implementation MyOpenGLView

void drawAnObject ()
{
 
    glViewport( -width/4, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width/1.0f, width/1.0f, -height, height, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(width/48, height/255, 1.0f);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glColor4f(1., 1., 1., 1.);
    
		glVertexPointer(2, GL_FLOAT, 2*sizeof(GLfloat), graph);
        glEnableClientState(GL_VERTEX_ARRAY);

        glDrawArrays(GL_LINE_STRIP, 0, 48);
        
        glDisableClientState(GL_VERTEX_ARRAY);
	
		glPopMatrix();
 
    
}

-(void) drawRect:(NSRect)bounds {
    
    NSLog(@"reshaping");
	
	//Get view dimensions
	NSRect baseRect = [self convertRectToBase:[self bounds]];
	width = baseRect.size.width;
	height = baseRect.size.height;

    [self setLayerContentsRedrawPolicy:1];
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    drawAnObject();
    glFlush();
    [[self openGLContext] flushBuffer];


    
}



// executes after everything in the xib/nib is initiallized
- (void)awakeFromNib {
	// we don't have a serial port open yet
	serialFileDescriptor = -1;
	readThreadRunning = FALSE;
	
	// first thing is to refresh the serial port list
	[self refreshSerialList:@"Select a Serial Port"];

    [self vref_dropdown_items:@"AVCC"];
    
	// now put the cursor in the text field
	[serialInputField becomeFirstResponder];
	
}

// open the serial port
//   - nil is returned on success
//   - an error message is returned otherwise
- (NSString *) openSerialPort: (NSString *)serialPortFile baud: (speed_t)baudRate {
	int success;
	
	// close the port if it is already open
	if (serialFileDescriptor != -1) {
		close(serialFileDescriptor);
		serialFileDescriptor = -1;
		
		// wait for the reading thread to die
		while(readThreadRunning);
		
		// re-opening the same port REALLY fast will fail spectacularly... better to sleep a sec
		sleep(0.5);
	}
	
	// c-string path to serial-port file
	const char *bsdPath = [serialPortFile cStringUsingEncoding:NSUTF8StringEncoding];
	
	// Hold the original termios attributes we are setting
	struct termios options;
	
	// receive latency ( in microseconds )
	unsigned long mics = 3;
	
	// error message string
	NSMutableString *errorMessage = nil;
	
	// open the port
	//     O_NONBLOCK causes the port to open without any delay (we'll block with another call)
	serialFileDescriptor = open(bsdPath, O_RDWR | O_NOCTTY | O_NONBLOCK );
	
	if (serialFileDescriptor == -1) {
		// check if the port opened correctly
		errorMessage = @"Error: couldn't open serial port";
	} else {
		// TIOCEXCL causes blocking of non-root processes on this serial-port
		success = ioctl(serialFileDescriptor, TIOCEXCL);
		if ( success == -1) {
			errorMessage = @"Error: couldn't obtain lock on serial port";
		} else {
			success = fcntl(serialFileDescriptor, F_SETFL, 0);
			if ( success == -1) {
				// clear the O_NONBLOCK flag; all calls from here on out are blocking for non-root processes
				errorMessage = @"Error: couldn't obtain lock on serial port";
			} else {
				// Get the current options and save them so we can restore the default settings later.
				success = tcgetattr(serialFileDescriptor, &gOriginalTTYAttrs);
				if ( success == -1) {
					errorMessage = @"Error: couldn't get serial attributes";
				} else {
					// copy the old termios settings into the current
					//   you want to do this so that you get all the control characters assigned
					options = gOriginalTTYAttrs;
					
					/*
					 cfmakeraw(&options) is equivilent to:
					 options->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
					 options->c_oflag &= ~OPOST;
					 options->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
					 options->c_cflag &= ~(CSIZE | PARENB);
					 options->c_cflag |= CS8;
					 */
					cfmakeraw(&options);
					
					// set tty attributes (raw-mode in this case)
					success = tcsetattr(serialFileDescriptor, TCSANOW, &options);
					if ( success == -1) {
						errorMessage = @"Error: coudln't set serial attributes";
					} else {
						// Set baud rate (any arbitrary baud rate can be set this way)
						success = ioctl(serialFileDescriptor, IOSSIOSPEED, &baudRate);
						if ( success == -1) {
							errorMessage = @"Error: Baud Rate out of bounds";
						} else {
							// Set the receive latency (a.k.a. don't wait to buffer data)
							success = ioctl(serialFileDescriptor, IOSSDATALAT, &mics);
							if ( success == -1) {
								errorMessage = @"Error: coudln't set serial latency";
							}
						}
					}
				}
			}
		}
	}
	
	// make sure the port is closed if a problem happens
	if ((serialFileDescriptor != -1) && (errorMessage != nil)) {
		close(serialFileDescriptor);
		serialFileDescriptor = -1;
	}
	
	return errorMessage;
}

// updates the textarea for incoming text by appending text
- (void)appendToIncomingText: (id) text {
	// add the text to the textarea
	NSAttributedString* attrString = [[NSMutableAttributedString alloc] initWithString: text];
	NSTextStorage *textStorage = [serialOutputArea textStorage];
	[textStorage beginEditing];
	[textStorage appendAttributedString:attrString];
	[textStorage endEditing];
	[attrString release];
	
	// scroll to the bottom
	NSRange myRange;
	myRange.length = 1;
	myRange.location = [textStorage length];
	[serialOutputArea scrollRangeToVisible:myRange];
}


- (void)GraphData: (id) text {
    
}




// This selector/function will be called as another thread...
//  this thread will read from the serial port and exits when the port is closed
- (void)incomingTextUpdateThread: (NSThread *) parentThread {
	
	// create a pool so we can use regular Cocoa stuff
	//   child threads can't re-use the parent's autorelease pool
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	// mark that the thread is running
	readThreadRunning = TRUE;

	const int BUFFER_SIZE = 1020;
	char byte_buffer[BUFFER_SIZE]; // buffer for holding incoming data
	int numBytes=0; // number of bytes read during read
	NSString *text; // incoming text from the serial port
    
	// assign a high priority to this thread
	[NSThread setThreadPriority:1.0];
	
	// this will loop until the serial port closes
	while(TRUE) {
        //while(numBytes<=0){
        //    [self writeString:@"s"];
        //}
		// read() blocks until some data is available or the port is closed
		numBytes = read(serialFileDescriptor, byte_buffer, BUFFER_SIZE); // read up to the size of the buffer
		if(numBytes>0) {
			// create an NSString from the incoming bytes (the bytes aren't null terminated)
			text = [NSString stringWithCString:byte_buffer length:numBytes];
            
            // Set Girino_Ready variable to "true"
            if ([text rangeOfString:@"Girino ready"].location != NSNotFound || Girino_Ready==TRUE) {
                Girino_Ready=TRUE;
                [self writeString:@"s"];
                
            }
            else {
                Girino_Ready=FALSE;
                [self reset];
                
            }
            // Route incoming Serial Data to the appropriate locations
            // if command 's', send incoming data to graph
            // if other command, send to terminal feedback data
            // Solution to try: String parsing to check for key phrases 'setting' whatever to, 'command not
            //              found'. if key phrases found, output to data graph.
            
            
            if ([text rangeOfString:@"Girino ready"].location != NSNotFound ||
                [text rangeOfString:@"Buffer size:"].location != NSNotFound ||
                [text rangeOfString:@"Baud rate:"].location != NSNotFound ||
                [text rangeOfString:@"Wait duration:"].location != NSNotFound ||
                [text rangeOfString:@"Trigger event:"].location != NSNotFound ||
                [text rangeOfString:@"Threshold:"].location != NSNotFound ||
                [text rangeOfString:@"ERROR: Command not found, it was:"].location != NSNotFound ||
                [text rangeOfString:@"Setting"].location != NSNotFound  ||
                [text rangeOfString:@"conversions"].location != NSNotFound) {
                
                [self performSelectorOnMainThread:@selector(appendToIncomingText:)
                                       withObject:text
                                    waitUntilDone:YES];
            }
            else {
                i1++;
                int max = 0;
                int min = 0;
                for( int i = 0; i < 1020; i++) {
                    graph[i].x = (float) i;
                    graph[i].y = (float) byte_buffer[i];
                    //graph[i].y = (float)((int) 127.5*sinf((float)(i*0.02))+127.5);
                    
                    if ( i<50 ) {
                    if (graph[i].y >= max)
                        max = graph[i].y;
                    if (graph[i].y <= min)
                        min = graph[i].y;
                    }
                }
                
                wf_Current_MAX = max;
                wf_Current_MIN = min;
                max = 0;
                min = 0;
                
                [openGLView setNeedsDisplay:YES];
                [openGLView setNeedsDisplayInRect:_bounds];

            
            }
            
			// this text can't be directly sent to the text area from this thread
			//  BUT, we can call a selctor on the main thread.
			
		} else {
			break; // Stop the thread if there is an error
		}
	}
	
	// make sure the serial port is closed
	if (serialFileDescriptor != -1) {
		close(serialFileDescriptor);
		serialFileDescriptor = -1;
	}
	
	// mark that the thread has quit
	readThreadRunning = FALSE;
	
	// give back the pool
	[pool release];
}

- (void) refreshSerialList: (NSString *) selectedText {
	io_object_t serialPort;
	io_iterator_t serialPortIterator;
	
	// remove everything from the pull down list
	[serialListPullDown removeAllItems];
	
	// ask for all the serial ports
	IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching(kIOSerialBSDServiceValue), &serialPortIterator);
	
	// loop through all the serial ports and add them to the array
	while (serialPort = IOIteratorNext(serialPortIterator)) {
		[serialListPullDown addItemWithTitle:
         (NSString*)IORegistryEntryCreateCFProperty(serialPort, CFSTR(kIOCalloutDeviceKey),  kCFAllocatorDefault, 0)];
		IOObjectRelease(serialPort);
	}
	
	// add the selected text to the top
	[serialListPullDown insertItemWithTitle:selectedText atIndex:0];
	[serialListPullDown selectItemAtIndex:0];
	
	IOObjectRelease(serialPortIterator);
}

- (void) vref_dropdown_items: (NSString *) selectedText  {
    [vrefListPullDown removeAllItems];
    [vrefListPullDown addItemWithTitle:@"AREF"];
    [vrefListPullDown addItemWithTitle:@"AVCC"];
    [vrefListPullDown addItemWithTitle:@"Reserved"];
    [vrefListPullDown addItemWithTitle:@"Internal 1.1v"];
    [vrefListPullDown insertItemWithTitle:selectedText atIndex:0];
    [vrefListPullDown selectItemAtIndex:0];
    
    if (Girino_Ready) {
        if ([selectedText rangeOfString:@"AREF"].location != NSNotFound) {
            [self writeString:@"r0"];
        }
        else if ([selectedText rangeOfString:@"AVCC"].location != NSNotFound) {
            [self writeString:@"r1"];
        }
        else if ([selectedText rangeOfString:@"Reserved"].location != NSNotFound) {
            [self writeString:@"r2"];
        }
        else if ([selectedText rangeOfString:@"Internal 1.1v"].location != NSNotFound) {
            [self writeString:@"r3"];
        }
    }
    
}

// send a string to the serial port
- (void) writeString: (NSString *) str {
	if(serialFileDescriptor!=-1) {
		write(serialFileDescriptor, [str cStringUsingEncoding:NSUTF8StringEncoding], [str length]);
	} else {
		// make sure the user knows they should select a serial port
		[self appendToIncomingText:@"\n ERROR:  Select a Serial Port from the pull-down menu\n"];
	}
}

// send a byte to the serial port
- (void) writeByte: (uint8_t *) val {
	if(serialFileDescriptor!=-1) {
		write(serialFileDescriptor, val, 1);
	} else {
		// make sure the user knows they should select a serial port
		[self appendToIncomingText:@"\n ERROR:  Select a Serial Port from the pull-down menu\n"];
	}
}

// action sent when serial port selected
- (IBAction) serialPortSelected: (id) cntrl {
	// open the serial port
	NSString *error = [self openSerialPort: [serialListPullDown titleOfSelectedItem] baud:[baudInputField intValue]];
	
	if(error!=nil) {
		[self refreshSerialList:error];
		[self appendToIncomingText:error];
	} else {
		[self refreshSerialList:[serialListPullDown titleOfSelectedItem]];
		[self performSelectorInBackground:@selector(incomingTextUpdateThread:) withObject:[NSThread currentThread]];
	}
}

- (IBAction) vrefSelected: (id) cntrl {
    [self vref_dropdown_items:[vrefListPullDown titleOfSelectedItem]];
}
- (IBAction) prescaler_cmd: (NSSlider *) sldr {
    // The prescaler command uses the slider value
    // by using 2*2^n| int ( 0 <= n <= 6 ) to return
    // values of {2,4,8,16,32,64,128} combining with
    // the character 'r' => 'r2', 'r16' or 'r128'
    int cmdint = 2*pow(2, [sldr integerValue]);
    [self writeString:[NSString stringWithFormat:@"p%d", cmdint]];
}

- (IBAction) threshold_cmd: (NSSlider *) sldr {
    // Threshold set anywhere from 0 to 255
    int cmdint = (int) [sldr integerValue];
    [self writeString:[NSString stringWithFormat:@"t%d", cmdint]];
}

- (IBAction) trigger_cmd: (NSMatrix *) mtrx {
    // Trigger has 4 values, but this is only including PGT and NGT values
    NSButtonCell *cell = [mtrx selectedCell];
    int cmdint = (int)[cell tag];
    [self writeString:[NSString stringWithFormat:@"e%d", cmdint]];
}

- (IBAction) set_to_fifty_P: (NSButton *) cntrl {
    // Take the Current average between current Max and Min values
    // and set threshold to the rounded to the nearest integer value of the average
    int cmdint;
    float tgt_threshold = ((wf_Current_MAX+127.5f) + (wf_Current_MIN+127.5f))/2.0f;
    if ( tgt_threshold < floorf(tgt_threshold)+0.5f )
        cmdint = floorf(tgt_threshold);
    else
        cmdint = ceilf(tgt_threshold);
    
    [self appendToIncomingText:@"\n [GUI MSG] \n"];
    [self appendToIncomingText:[NSString stringWithFormat:@"MAX Reading: %f \n", wf_Current_MAX+127.5]];
    [self appendToIncomingText:[NSString stringWithFormat:@"50 percent: %d \n", cmdint]];
    [self appendToIncomingText:[NSString stringWithFormat:@"MIN Reading: %f \n", wf_Current_MIN+127.5]];
    [self appendToIncomingText:@"[END GUI MSG] \n"];
    
    [threshold_Slider setIntValue:cmdint];
    [self writeString:[NSString stringWithFormat:@"t%d", cmdint]];
}

- (IBAction) set_to_75_P: (NSButton *) cntrl {
    // Take the Current average between current Max and Min values
    // and set threshold to the rounded to the nearest integer value of the average
    int cmdint;
    float tgt_threshold = (wf_Current_MIN+127.5f)+(3.0f*(((wf_Current_MAX+127.5f) - (wf_Current_MIN+127.5f))/4.0f));
    if ( tgt_threshold < floorf(tgt_threshold)+0.5f )
        cmdint = floorf(tgt_threshold);
    else
        cmdint = ceilf(tgt_threshold);
    
    [self appendToIncomingText:@"\n [GUI MSG] \n"];
    [self appendToIncomingText:[NSString stringWithFormat:@"MAX Reading: %f \n", wf_Current_MAX+127.5]];
    [self appendToIncomingText:[NSString stringWithFormat:@"75 percent: %d \n", cmdint]];
    [self appendToIncomingText:[NSString stringWithFormat:@"MIN Reading: %f \n", wf_Current_MIN+127.5]];
    [self appendToIncomingText:@"[END GUI MSG] \n"];
    
    [threshold_Slider setIntValue:cmdint];
    [self writeString:[NSString stringWithFormat:@"t%d", cmdint]];
}

- (IBAction) set_to_25_P: (NSButton *) cntrl {
    // Take the Current average between current Max and Min values
    // and set threshold to the rounded to the nearest integer value of the average
    int cmdint;
    float tgt_threshold = ((wf_Current_MAX+127.5f) + (wf_Current_MIN+127.5f))/4.0f;
    if ( tgt_threshold < floorf(tgt_threshold)+0.5f )
        cmdint = floorf(tgt_threshold);
    else
        cmdint = ceilf(tgt_threshold);
    
    [self appendToIncomingText:@"\n [GUI MSG] \n"];
    [self appendToIncomingText:[NSString stringWithFormat:@"MAX Reading: %f \n", wf_Current_MAX+127.5]];
    [self appendToIncomingText:[NSString stringWithFormat:@"25 percent: %d \n", cmdint]];
    [self appendToIncomingText:[NSString stringWithFormat:@"MIN Reading: %f \n", wf_Current_MIN+127.5]];
    [self appendToIncomingText:@"[END GUI MSG] \n"];
    
    [threshold_Slider setIntValue:cmdint];
    [self writeString:[NSString stringWithFormat:@"t%d", cmdint]];
}





// action from baud rate change
- (IBAction) baudAction: (id) cntrl {
	if (serialFileDescriptor != -1) {
		speed_t baudRate = [baudInputField intValue];
		
		// if the new baud rate isn't possible, refresh the serial list
		//   this will also deselect the current serial port
		if(ioctl(serialFileDescriptor, IOSSIOSPEED, &baudRate)==-1) {
			[self refreshSerialList:@"Error: Baud Rate out of bounds"];
			[self appendToIncomingText:@"Error: Baud Rate out of bounds"];
		}
	}
}

// action from refresh button
- (IBAction) refreshAction: (id) cntrl {
	[self refreshSerialList:@"Select a Serial Port"];
	
	// close serial port if open
	if (serialFileDescriptor != -1) {
		close(serialFileDescriptor);
		serialFileDescriptor = -1;
	}
}

// action from send button and on return in the text field
- (IBAction) sendText: (id) cntrl {
	// send the text to the Arduino
	[self writeString:[serialInputField stringValue]];
	
	// blank the field
	[serialInputField setTitleWithMnemonic:@""];
}

// action from send button and on return in the text field
- (IBAction) sliderChange: (NSSlider *) sldr {
	uint8_t val = [sldr intValue];
	[self writeByte:&val];
}


// action from the A button
- (IBAction) hitAButton: (NSButton *) btn {
	[self writeString:@"A"];
}

// action from the B button
- (IBAction) hitBButton: (NSButton *) btn {
	[self writeString:@"B"];
}

// action from the C button
- (IBAction) hitCButton: (NSButton *) btn {
	[self writeString:@"C"];
}

// action from the reset button
- (IBAction) resetButton: (NSButton *) btn {
	// set and clear DTR to reset an arduino
	struct timespec interval = {0,100000000}, remainder;
	if(serialFileDescriptor!=-1) {
		ioctl(serialFileDescriptor, TIOCSDTR);
		nanosleep(&interval, &remainder); // wait 0.1 seconds
		ioctl(serialFileDescriptor, TIOCCDTR);
	}
}
-(void) reset {
    // set and clear DTR to reset an arduino
	struct timespec interval = {0,100000000}, remainder;
	if(serialFileDescriptor!=-1) {
		ioctl(serialFileDescriptor, TIOCSDTR);
		nanosleep(&interval, &remainder); // wait 0.1 seconds
		ioctl(serialFileDescriptor, TIOCCDTR);
	}

}


@end
