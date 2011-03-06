/*
 *  Node.cpp
 *  iPhoneEmptyExample
 *
 *  Created by hansi on 28.01.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "MUI.h"

void mui::Container::add( Container * c ){
	children.push_back( c ); 
	c->parent = this; 
}

void mui::Container::drawBackground(){
}


void mui::Container::handleDraw(){
	if( !visible ) return; 
	
	ofPushMatrix(); 
	ofTranslate( x, y ); 
	
	if( !opaque ) drawBackground(); 
	draw(); 
	
	if( MUI_DEBUG_DRAW ){
		ofNoFill(); 
		ofSetColor( 255, 0, 0 );
		ofRect( 0, 0, width, height ); 
	}
	
	std::vector<Container*>::reverse_iterator it = children.rbegin();
	while( it != children.rend() ) {
		(*it)->handleDraw(); 
		++it;
	}

	ofPopMatrix(); 
}


void mui::Container::handleUpdate(){
	update(); 
	
	std::vector<Container*>::iterator it = children.begin();
	while( it != children.end() ) {
		(*it)->handleUpdate(); 
		++it;
	}
}

mui::Container * mui::Container::handleTouchDown( ofTouchEventArgs &touch ){
	if( !visible ) return NULL; 

	
	string depth = ""; 
	Container * p = this; 
	while( p->parent != NULL ) p = p->parent, depth = "  " + depth; 
	
	//cout << "do kids care?" << touch.x << ", " << touch.y << "--" << endl; 
	if( touch.x >= x && touch.x <= x+width && touch.y >= y && touch.y <= y+height ){
		cout << depth << "> checking kids for: " << this->toString() << endl; 
		float x = this->x; 
		float y = this->y; 
		Container * touched; 
		
		touch.x -= x;
		touch.y -= y;
		
		std::vector<Container*>::iterator it = children.begin();
		while( it != children.end() ){
			touched = (*it)->handleTouchDown( touch ); 
			if( touched != NULL ){
				// that container is touched! 
				touch.x += x;
				touch.y += y;
				return touched;
			}
			
			++it;
		}
		
		cout << depth << "> kids didn't care, myself? " << this->toString() << endl; 
		if( !ignoreEvents ){
			if( singleTouchId == -1 ){
				singleTouchId = touch.id; 
			}
			
			if( !singleTouch || ( singleTouch && singleTouchId == touch.id ) ){
				touchDown( touch ); 
			}
			
			return this; 
		}
	}
	
	return NULL; 
}

mui::Container * mui::Container::handleTouchMoved( ofTouchEventArgs &touch ){
	if( !visible ) return NULL; 
	
	
	//cout << "do kids care?" << touch.x << ", " << touch.y << "--" << endl; 
	if( touch.x >= x && touch.x <= x+width && touch.y >= y && touch.y <= y+height ){
		float x = this->x;
		float y = this->y;
		Container * touched;
		
		touch.x -= x;
		touch.y -= y;
		
		std::vector<Container*>::iterator it = children.begin();
		while( it != children.end() ) {
			touched = (*it)->handleTouchMoved( touch ); 
			if( touched != NULL ){
				// that container is touched! 
				touch.x += x;
				touch.y += y;
				return touched;
			}
			
			++it;
		}
		
		if( !ignoreEvents ){
			if( !singleTouch || ( singleTouch && singleTouchId == touch.id ) ){
				touchMoved( touch );
			}
			
			return this; 
		}
	}
	
	return NULL;
}

mui::Container * mui::Container::handleTouchDoubleTap( ofTouchEventArgs &touch ){
	if( !visible ) return NULL; 
	
	
	//cout << "do kids care?" << touch.x << ", " << touch.y << "--" << endl; 
	if( touch.x >= x && touch.x <= x+width && touch.y >= y && touch.y <= y+height ){
		float x = this->x;
		float y = this->y;
		Container * touched; 
		
		touch.x -= x;
		touch.y -= y;
		
		std::vector<Container*>::iterator it = children.begin();
		while( it != children.end() ) {
			touched = (*it)->handleTouchDoubleTap( touch ); 
			if( touched != NULL ){
				// that container is touched! 
				touch.x += x;
				touch.y += y;
				return touched;
			}
			
			++it;
		}
		
		if( !ignoreEvents ){
			if( !singleTouch || ( singleTouch && singleTouchId == touch.id ) ){
				touchDoubleTap( touch ); 
			}
			
			return this; 
		}
	}
	
	return NULL; 
}

mui::Container * mui::Container::handleTouchUp( ofTouchEventArgs &touch ){
	if( !visible ) return NULL; 
	
	
	//cout << "do kids care?" << touch.x << ", " << touch.y << "--" << endl; 
	if( touch.x >= x && touch.x <= x+width && touch.y >= y && touch.y <= y+height ){
		float x = this->x; 
		float y = this->y; 
		Container * touched; 
		
		touch.x -= x;
		touch.y -= y;
		
		std::vector<Container*>::iterator it = children.begin();
		while( it != children.end() ) {
			touched = (*it)->handleTouchUp( touch ); 
			if( touched != NULL ){
				// that container is touched! 
				touch.x += x;
				touch.y += y;
				return touched;
			}
			
			++it;
		}
		
		if( !ignoreEvents ){
			if( !singleTouch || ( singleTouch && singleTouchId == touch.id ) ){
				touchUp( touch ); 
				singleTouchId = -1;
			}
			return this; 
		}
	}
	
	return NULL; 
}

ofPoint mui::Container::getGlobalPosition(){
	ofPoint result; 
	
	float x = 0; 
	float y = 0; 
	
	Container * parent = this; 
	while( parent != NULL ){
		x += parent->x; 
		y += parent->y; 
		parent = parent->parent; 
	}
	
	result.x = x; 
	result.y = y; 
	
	return result; 
}


string mui::Container::toString(){
	return 
		string("Container") + 
		"[ name=" + name + 
		", x=" + ofToString( x, 0 ) + 
		", y=" + ofToString( y, 0 ) +
		", y=" + ofToString( y, 0 ) +
		", w=" + ofToString( width, 0 ) +
		", h=" + ofToString( height, 0 ) + 
		"]"; 
}

