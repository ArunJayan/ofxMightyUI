/*
 *  Button.cpp
 *  iPhoneEmptyExample
 *
 *  Created by hansi on 29.01.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 * ------------------------------
 *  PLEASE READ BEFORE YOU CODE:
 *
 *  This is a simple container thingie.
 *  You can use it exactly like an OF main app, there are only small differences in how touch events work.
 *  1. If you don't want an event to propagate to other containers just do a "return true;" at then end.
 *  2. A container will only be notified of events that happen within it's bounds
 *  3. If you do a "return true" at the end of touchDown() your container will be remembered and will then
 *     receive all moved(), doubleTap() and touchUp() events, even if they happen outside the container.
 *  4. The coordinates touch.x and touch.y are modified to match the drawing coordinate system, so 0/0 is at the
 *     left top corner of your container, not top left corner of your application.
 */


#include "MUI.h"


//--------------------------------------------------------------
void mui::ToggleButton::init( std::string title ){
	label = new Label( title, 0, 0, width, height );
	label->horizontalAlign = Center;
	label->verticalAlign = Middle;
	label->fg.r = label->fg.g = label->fg.b = 255;
	label->fontSize = 12;
	label->commit();
	bg.r = bg.g = bg.b = 125;
	bg.a = 0;
	add( label );
	opaque = true;
	name = "togglebutton-" + title;
}

//--------------------------------------------------------------
void mui::ToggleButton::update(){
	label->width = width;
	label->height = height;
	if( selected ){
		bg = ofColor( 255 );
		label->fg = ofColor( 0 );
	}
	else{
		bg = ofColor( 125, 50 );
		label->fg = ofColor( 255 );
	}
}


//--------------------------------------------------------------
void mui::ToggleButton::draw(){
}


//--------------------------------------------------------------
void mui::ToggleButton::drawBackground(){
	if( bg.a > 0 ){
		ofFill();
		ofSetColor( bg.r, bg.g, bg.b, bg.a );
		//Helpers::roundedRect( 0, 0, width, height, 5 );
		ofRect( 0, 0, width, height );
		
		if( pressed ){
//			ofSetColor( bg.r/2, bg.g/2, bg.b/2, bg.a );
		}
		else if( selected ){
		}
		
		/*ofNoFill();
		 Helpers::roundedRect( 0, 0, width, height, 5 );
		 
		 ofFill(); */
	}
}


//--------------------------------------------------------------
void mui::ToggleButton::touchDown( ofTouchEventArgs &touch ){
	pressed = true;
}


//--------------------------------------------------------------
void mui::ToggleButton::touchMoved( ofTouchEventArgs &touch ){
	pressed = true;
}


//--------------------------------------------------------------
void mui::ToggleButton::touchMovedOutside( ofTouchEventArgs &touch ){
	pressed = false;
}


//--------------------------------------------------------------
void mui::ToggleButton::touchUp( ofTouchEventArgs &touch ){
	selected = !selected;
	pressed = false;
	onPress( this, touch );
}


//--------------------------------------------------------------
void mui::ToggleButton::touchUpOutside( ofTouchEventArgs &touch ){
	pressed = false;
}


//--------------------------------------------------------------
void mui::ToggleButton::touchDoubleTap( ofTouchEventArgs &touch ){
}


//--------------------------------------------------------------
void mui::ToggleButton::touchCanceled( ofTouchEventArgs &touch ){
	pressed = false;
}
