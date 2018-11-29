// CS 349 Fall 2018
// A1: Breakout code sample
// You may use any or all of this code in your assignment!
// See makefile for compiling instructions

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sstream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;

// X11 structures
Display* display;
Window window;

// fixed frames per second animation
int FPS = 60;

// get current time
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

struct Brick {
	int x;
	int y;
	bool status;
};

// entry point
int main( int argc, char *argv[] ) {
	FPS = atoi(argv[1]);
	float ballspeed = 1+ 0.5*(atoi(argv[2]) - 1);
	// create window
	display = XOpenDisplay("");
	if (display == NULL) exit (-1);
	int screennum = DefaultScreen(display);
	int screenwidth = 800;
	int screenheight = 600;
	int rows = 5;
	int columns = 10;
	Brick bricks [rows][columns];
	int paddleWidth = 80;
	int paddleHeight = 10;
	int paddlePadding = 40;
	int brickWidth = 50;
	int brickHeight = 30;
	int columnbuffer = 10;
	int rowbuffer = 10;
	int leftbuffer = (screenwidth - columns*brickWidth - (columns - 1) * columnbuffer) / 2;
	int topbuffer = 20;
	int score = 0;
	bool paddleR = false;
	bool paddleL = false;
	long background = WhitePixel(display, screennum);
	long foreground = BlackPixel(display, screennum);
	window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                            10, 10, screenwidth, screenheight, 2, foreground, background);

	for(int i = 0; i < rows; i++){
		for(int j = 0; j < columns; j++){
			bricks[i][j].status = 1;
			bricks[i][j].x = leftbuffer + j*(brickWidth+columnbuffer);
			bricks[i][j].y = topbuffer + i*(brickHeight+rowbuffer);
		}
	}

	// set events to monitor and display window
	XSelectInput(display, window, ButtonPressMask | KeyPressMask | KeyReleaseMask);
	XMapRaised(display, window);
	XFlush(display);

	Colormap cmap = XDefaultColormap(display, screennum);

	XColor xcolour, xcolour2, xcolour3;
			// I guess XParseColor will work here
	xcolour.red = 32000; xcolour.green = 65000; xcolour.blue = 32000;
	xcolour2.red = 0; xcolour2.green = 0; xcolour2.blue = 0;
	xcolour.flags = DoRed | DoGreen | DoBlue;
	xcolour2.flags = DoRed | DoGreen | DoBlue;

	xcolour3.red = 0; xcolour3.green = 0; xcolour3.blue = 65000;
	xcolour3.flags = DoRed | DoGreen | DoBlue;

	// ball postition, size, and velocity
	XPoint ballPos;
	ballPos.x = screenwidth/2;
	ballPos.y = screenheight - paddlePadding * 2;
	int ballSize = 10;


	XPoint ballDir;
	ballDir.x = ballspeed;
	ballDir.y = -ballspeed;

	int paddlespeed = 5;

	// block position, size
	XPoint rectPos;
	rectPos.x = (screenwidth - brickWidth)/2;
	rectPos.y = screenheight - paddlePadding;

	// create gc for drawing
	GC gc = XCreateGC(display, window, 0, 0);

	XWindowAttributes w;
	XGetWindowAttributes(display, window, &w);

	// save time of last window paint
	unsigned long lastRepaint = 0;

	// event handle for current event
	XEvent event;
	unsigned long startpaint = 0;

	int depth = DefaultDepth(display, DefaultScreen(display));
	Pixmap buffer = XCreatePixmap(display, window, screenwidth, screenheight, depth);
	unsigned long start = now();
	unsigned long end = now();
	bool paddlebounce = false;
	bool pause = true;
	// event loop
	while ( true ) {

		// process if we have any events
		
	
		if (XPending(display) > 0) { 
			XNextEvent( display, &event ); 
			bool keyR = true;
			if (event.type == KeyRelease && XEventsQueued(display, QueuedAfterReading)){
				XEvent nev;
				XPeekEvent(display, &nev);

				if (nev.type == KeyPress && nev.xkey.time == event.xkey.time && nev.xkey.keycode == event.xkey.keycode){
			    	keyR = false;
				}
			}

			switch ( event.type ) {

				// mouse button press
				case ButtonPress:
					cout << "CLICK" << endl;
					break;

				case KeyPress: {// any keypress
					KeySym key;
					char text[10];
					int i = XLookupString( (XKeyEvent*)&event, text, 10, &key, 0 );

					// move right
					if ( i == 1 && text[0] == 'd' ) {
						paddleR = true;
						paddleL = false;
					}

					// move left
					if ( i == 1 && text[0] == 'a' ) {
						paddleL = true;
						paddleR = false;
					}

					if ( i == 1 && text[0] == 'p' ) {
						pause = !pause;
					}

					// quit game
					if ( i == 1 && text[0] == 'q' ) {
						XCloseDisplay(display);
						exit(0);
					}
					break;
				}
				case KeyRelease:
					KeySym key;
					char text[10];
					int i = XLookupString( (XKeyEvent*)&event, text, 10, &key, 0 );
					if(keyR){
						if(i == 1 && text[0] == 'd'){
							paddleR = false;
						}
						if(i == 1 && text[0] == 'a'){
							paddleL = false;
						}
					}
				break;
			}
		}
			

			// get current time in microsecond
		end = now();
		if (end - startpaint > 1000000 / FPS) {
			startpaint = now();
		if(pause){
			Pixmap pixmap;

			pixmap = buffer;

			XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
			XFillRectangle(display, pixmap, gc, 0, 0, screenwidth, screenheight);

			XAllocColor(display, cmap, &xcolour);

			XSetForeground(display, gc, xcolour.pixel);

			

			XDrawImageString(display, pixmap, gc, 100, 50, " Brick Breaker ",15);
			XDrawImageString(display, pixmap, gc, 100, 100, " By Colby Parsons ",18);
			XDrawImageString(display, pixmap, gc, 100, 150, " ID #20665652 ",14);
			XDrawImageString(display, pixmap, gc, 100, 200, " D moves the paddle right ",26);
			XDrawImageString(display, pixmap, gc, 100, 250, " A moves the paddle left ",25);
			XDrawImageString(display, pixmap, gc, 100, 300, " P toggles this pause menu ",27);
			XDrawImageString(display, pixmap, gc, 100, 350, " Q quits the game ",18);
			XCopyArea(display, pixmap, window, gc,
				          0, 0, screenwidth, screenheight,  // region of pixmap to copy
				          0, 0);


			start = now();
		}else{
		
		unsigned long delta = now() - start;
		int d = delta/10000;
		start += d*10000;

		float ballX = ballPos.x;
		float ballY = ballPos.y - ballSize/2;

		if(paddleL){
			if(rectPos.x - d*paddlespeed < 0){
				rectPos.x = 0;
			}else{
				rectPos.x -= d*paddlespeed;
			}
		} else if(paddleR){
			if(rectPos.x + d*paddlespeed + paddleWidth > screenwidth){
				rectPos.x = screenwidth- paddleWidth;
			}else{
				rectPos.x += d*paddlespeed;
			}
		}
		paddlebounce = false;
		for(int k = 1; k <=d;k++){
			if(ballX + k*ballDir.x > rectPos.x && ballX + k*ballDir.x <  rectPos.x + paddleWidth && ballY + 5 + k*ballDir.y > rectPos.y && ballY + k*ballDir.y < rectPos.y + paddleHeight){
				if(ballY < rectPos.y && ballDir.y > 0){
					ballDir.y = -ballDir.y;
					d -=k;
					paddlebounce = true;
					break;
				}
			}
		}
		
		if(!paddlebounce){
			for(int i = 0; i < rows; i++){
				for(int j = 0; j < columns; j++){
					if(bricks[i][j].status == 1){
						for(int k = 1; k <=d;k++){
							if(ballX + k*ballDir.x > bricks[i][j].x && ballX + k*ballDir.x <  bricks[i][j].x + brickWidth && ballY + 5 + k*ballDir.y > bricks[i][j].y && ballY + k*ballDir.y < bricks[i][j].y + brickHeight){
								if((ballX-5 < bricks[i][j].x && ballDir.x > 0)||(ballX+5 > bricks[i][j].x + brickWidth && ballDir.x < 0)){
									ballDir.x = -ballDir.x;

								}
								if((ballY-5 < bricks[i][j].y && ballDir.y > 0)||(ballY + 10 > bricks[i][j].y + brickHeight && ballDir.y < 0)){
									ballDir.y = -ballDir.y;
								}
								bricks[i][j].status = 0;
								score++;
								if(score % (rows*columns) == 0) {
			                        for(int i = 0; i < rows; i++){
										for(int j = 0; j < columns; j++){
											bricks[i][j].status = 1;
										}
									}
									ballPos.x = screenwidth/2;
									ballPos.y = screenheight - paddlePadding * 2;
									ballDir.x = ballspeed;
									ballDir.y = -ballspeed;
		                    	}else{
		                    		d -=k;
		                    	}
		                    	break;
							}
						}
					}
				}
			}
		}

		// update ball position
		ballPos.x += d*ballDir.x;
		ballPos.y += d*ballDir.y;

		// bounce ball
		if (ballPos.x + ballSize/2 > w.width){
			ballDir.x = -ballspeed;
		}
		
		if(ballPos.x - ballSize/2 < 0){
			ballDir.x = ballspeed;
		}

		if (ballPos.y + ballSize/2 > w.height){
			for(int i = 0; i < rows; i++){
				for(int j = 0; j < columns; j++){
					bricks[i][j].status = 1;
				}
			}
			ballPos.x = screenwidth/2;
			ballPos.y = screenheight - paddlePadding * 2;
			ballDir.x = ballspeed;
			ballDir.y = -ballspeed;
			score = 0;
			pause = true;
		}
		if(ballPos.y - ballSize/2 < 0){
			ballDir.y = ballspeed;
		}

			Pixmap pixmap;

			pixmap = buffer;

			XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
			XFillRectangle(display, pixmap, gc,
				               0, 0, screenwidth, screenheight);
			
			

			XAllocColor(display, cmap, &xcolour3);
			XSetForeground(display, gc, xcolour3.pixel);

			XFillRectangle(display, pixmap, gc, rectPos.x, rectPos.y, paddleWidth, paddleHeight);

			
			XAllocColor(display, cmap, &xcolour);

			XSetForeground(display, gc, xcolour.pixel);


			std::ostringstream oss;
			std::string scoreStr = " Score: ";
			oss << score << " ";
			scoreStr += oss.str();

			const char * c = scoreStr.c_str();
			
			XDrawImageString(display, pixmap, gc, 5, 20, c, scoreStr.length());

			for(int i = 0; i < rows; i++){
				for(int j = 0; j < columns; j++){
					if(bricks[i][j].status){
						XFillRectangle(display, pixmap, gc, bricks[i][j].x, bricks[i][j].y, brickWidth, brickHeight);
					}
				}
			}
			XAllocColor(display, cmap, &xcolour2);
			XSetForeground(display, gc, xcolour2.pixel);

			// draw ball from centre
			XFillArc(display, pixmap, gc, 
				ballPos.x - ballSize/2, 
				ballPos.y - ballSize/2, 
				ballSize, ballSize,
				0, 360*64);

			XCopyArea(display, pixmap, window, gc,
				          0, 0, screenwidth, screenheight,  // region of pixmap to copy
				          0, 0);
			}

			XFlush( display );

			lastRepaint = now(); // remember when the paint happened
		}

		// IMPORTANT: sleep for a bit to let other processes work
		if (XPending(display) == 0) {
			int time = now() - startpaint;
			if(time < 1000000 / FPS && time > 100){
				usleep((int)(1000000 / FPS - time));
			}
		}
	}
	XCloseDisplay(display);
}
