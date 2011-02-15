/*
 *  InternalWindow.cpp
 *  iPhoneEmptyExample
 *
 *  Created by hansi on 29.01.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 */

#include "MUI.h"
#include <algorithm>

//--------------------------------------------------------------
void mui::InternalWindow::init( std::string title ){
	label = new Label( title, 0, 0, width, 44 ); 
	label->horizontalAlign = Center; 
	label->verticalAlign = Middle; 
	label->fg.r = label->fg.g = label->fg.b = 255; 
	label->fontSize = 16; 
	label->commit(); 
	
	add( label ); 
}

//--------------------------------------------------------------
void mui::InternalWindow::update(){
	label->width = width; 
	
	if( leftBarButton != NULL ){
		leftBarButton->width = leftBarButton->label->boundingBox.width + 10; 
		leftBarButton->height = 30; 
		leftBarButton->x = leftBarButton->y = 7;
	}
	
	if( rightBarButton != NULL ){
		rightBarButton->width = rightBarButton->label->boundingBox.width + 10; 
		rightBarButton->height = 30; 
		rightBarButton->x = width - 7 - rightBarButton->width; 
		rightBarButton->y = 7;
	}
}


//--------------------------------------------------------------
void mui::InternalWindow::draw(){	
}


//--------------------------------------------------------------
void mui::InternalWindow::drawBackground(){
	ofSetColor( 255, 255, 255, 200 ); 
	ofFill(); 
	Helpers::roundedRect( 0, 0, width, height, radius );
	ofNoFill(); 
	//Helpers::roundedRect( 1, 22, width-2, height-22, radius ); 
	
	ofSetColor( 255, 255, 255 ); 
	Helpers::getImage( "titlebar_left" )->draw( 0, 0, 5, 44 ); 
	Helpers::getImage( "titlebar_center" )->draw( 5, 0, width - 9, 44 ); 
	Helpers::getImage( "titlebar_right" )->draw( width-5, 0, 5, 44 ); 
}


//--------------------------------------------------------------
void mui::InternalWindow::setLeftBarButton( BarButton * leftBarButton ){
	if( this->leftBarButton != NULL ){
		//children.remove( leftBarButton );
		vector<Container*>::iterator it = find( children.begin(), children.end(), this->leftBarButton ); 
		if( it != children.end() ) children.erase( it ); 
	}
	
	add( leftBarButton ); 
	this->leftBarButton = leftBarButton; 
}


//--------------------------------------------------------------
void mui::InternalWindow::setRightBarButton( BarButton * rightBarButton ){
	if( this->rightBarButton != NULL ){
		vector<Container*>::iterator it = find( children.begin(), children.end(), this->rightBarButton ); 
		if( it != children.end() ) children.erase( it ); 
	}
	
	add( rightBarButton ); 
	this->rightBarButton = rightBarButton; 
}


//--------------------------------------------------------------
bool mui::InternalWindow::touchDown( ofTouchEventArgs &touch ){
	if( touch.y <= 44 ){
		dragX = touch.x; 
		dragY = touch.y; 
		return true; 
	}
	
	return false; 
}


//--------------------------------------------------------------
bool mui::InternalWindow::touchMoved( ofTouchEventArgs &touch ){
	int deltaX = touch.x - dragX; 
	int deltaY = touch.y - dragY; 
	x += deltaX; 
	y += deltaY; 
	//dragX += deltaX; 
	//dragY += deltaY; 
	return true; 
}


//--------------------------------------------------------------
bool mui::InternalWindow::touchUp( ofTouchEventArgs &touch ){
	return false; 
}


//--------------------------------------------------------------
bool mui::InternalWindow::touchDoubleTap( ofTouchEventArgs &touch ){
	return false; 
}