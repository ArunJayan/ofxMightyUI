/*
 *  Node.cpp
 *  iPhoneEmptyExample
 *
 *  Created by hansi on 28.01.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Container.h"
#include "MuiConfig.h"
#include "Root.h"

mui::Container::Container( float x_, float y_, float width_, float height_ ) : x(x_), y(y_), width(width_), height(height_), opaque(false), parent(NULL), layoutHandler(NULL), visible(true), ignoreEvents(false), singleTouch(true), name( "Container" ), singleTouchId( -1 ), focusTransferable(true),bg(0,0,0,0), allowSubpixelTranslations(true), drawDirty(false), userData(NULL){
	//for( int i = 0; i < OF_MAX_TOUCHES; i++ ){
	//	startedInside[i] = false;
	//}
};

//--------------------------------------------------------------
mui::Container::~Container(){
	MUI_ROOT->removeFromResponders( this );
}


//--------------------------------------------------------------
void mui::Container::add( Container * c, int index ){
    if( index == -1 ){
        children.push_back( c );
        c->parent = this;
    }
    else{
        vector<Container*>::iterator it = children.begin();
        it += index;
        children.insert( it, c );
        c->parent = this;
    }
}

//--------------------------------------------------------------
void mui::Container::remove( Container * c ){
    vector<Container*>::iterator it = find( children.begin(), children.end(), c );
	MUI_ROOT->removeFromResponders( c );
    if( it != children.end() ){
        children.erase( it );
    }
}

//--------------------------------------------------------------
void mui::Container::remove(){
	MUI_ROOT->removeFromResponders( this );
    if( parent != NULL ){
        parent->remove( this );
    }
}

void mui::Container::setBounds( float x, float y, float w, float h ){
	this->x = x;
	this->y = y;
	this->width = w;
	this->height = h;
	handleLayout();
}

//--------------------------------------------------------------
void mui::Container::layout(){
	// layout managers might have been a fucking stupid idea.
	// or maybe not. this is definitely gonna change!
	if( layoutHandler != NULL ){
		layoutHandler->layout( this );
	}
}

void mui::Container::handleLayout(){
	// does this order make sense?
	if( layoutHandler == NULL ){
		layout();
	}
	else{
		layoutHandler->layout(this);
	}

	vector<mui::Container*>::iterator it = children.begin();
	while( it != children.end() ){
		(*it)->handleLayout();
		++it;
	}
}


//--------------------------------------------------------------
void mui::Container::drawBackground(){
    if( bg.a > 0 ){
		ofFill();
        ofSetColor( bg );
        ofRect( 0, 0, width, height );
        ofSetColor( 255 );
    }
}


//--------------------------------------------------------------
void mui::Container::handleDraw(){
	if( !visible ) return;
	
	ofPushMatrix();
	if( allowSubpixelTranslations ) ofTranslate( x, y );
	else ofTranslate( (int)x, (int)y );
	
	if( opaque ) drawBackground();
	draw();
	
	if( MUI_DEBUG_DRAW ){
		ofNoFill();
		ofSetColor( 255, 0, 0 );
		ofRect( 0, 0, width, height );
	}
	
	std::vector<Container*>::iterator it = children.begin();
	while( it != children.end() ) {
        (*it)->handleDraw();
		++it;
	}
    
	ofPopMatrix();
}


//--------------------------------------------------------------
void mui::Container::handleUpdate(){
	update();
	
	std::vector<Container*>::iterator it = children.begin();
	while( it != children.end() ) {
        //Container * child = (*it); // just for debugging ...
		(*it)->handleUpdate();
		++it;
	}
}


//--------------------------------------------------------------
mui::Container * mui::Container::handleTouchDown( ofTouchEventArgs &touch ){
	if( !visible ) return NULL;
    
	
	//string depth = "";
	//Container * p = this;
	//while( p->parent != NULL ) p = p->parent, depth = "  " + depth;
	
	if( touch.x >= 0 && touch.x <= width && touch.y >= 0 && touch.y <= height ){
		float x, y;
		Container * touched;
		
		std::vector<Container*>::reverse_iterator it = children.rbegin();
		while( it != children.rend() ){
			touch.x -= ( x = (*it)->x );
			touch.y -= ( y = (*it)->y );
			touched = (*it)->handleTouchDown( touch );
			touch.x += x;
			touch.y += y;
			
			if( touched != NULL ){
				return touched;
			}
			
			++it;
		}
		
		if( !ignoreEvents ){
			if( singleTouchId == -1 ){
				singleTouchId = touch.id;
			}
			
			if( !singleTouch || ( singleTouch && singleTouchId == touch.id ) ){
				touchDown( touch );
			}
			
			if( MUI_ROOT->respondingContainer[touch.id] != NULL ){
				return MUI_ROOT->respondingContainer[touch.id];
			}
			else{
				return this;
			}
		}
	}
	
	return NULL;
}


//--------------------------------------------------------------
mui::Container * mui::Container::handleTouchMoved( ofTouchEventArgs &touch ){
	if( !visible ) return NULL;
	
	
	if( touch.x >= 0 && touch.x <= width && touch.y >= 0 && touch.y <= height ){
		float x, y;
		Container * touched;
		
		std::vector<Container*>::reverse_iterator it = children.rbegin();
		while( it != children.rend() ) {
			touch.x -= ( x = (*it)->x );
			touch.y -= ( y = (*it)->y );
			touched = (*it)->handleTouchMoved( touch );
			touch.x += x;
			touch.y += y;
			
			if( touched != NULL ){
				// that container is touched!
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


//--------------------------------------------------------------
mui::Container * mui::Container::handleTouchDoubleTap( ofTouchEventArgs &touch ){
	if( !visible ) return NULL;
	
	
	if( touch.x >= 0 && touch.x <= width && touch.y >= 0 && touch.y <= height ){
		float x, y;
		Container * touched;
		
		std::vector<Container*>::reverse_iterator it = children.rbegin();
		while( it != children.rend() ) {
			touch.x -= ( x = (*it)->x );
			touch.y -= ( y = (*it)->y );
			touched = (*it)->handleTouchDoubleTap( touch );
			touch.x += x;
			touch.y += y;
			
			if( touched != NULL ){
				// that container is touched!
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


//--------------------------------------------------------------
mui::Container * mui::Container::handleTouchUp( ofTouchEventArgs &touch ){
	if( !visible ) return NULL;
	
	
	if( touch.x >= 0 && touch.x <= width && touch.y >= 0 && touch.y <= height ){
		float x, y;
		Container * touched;
		
		std::vector<Container*>::reverse_iterator it = children.rbegin();
		while( it != children.rend() ) {
			touch.x -= ( x = (*it)->x );
			touch.y -= ( y = (*it)->y );
			touched = (*it)->handleTouchUp( touch );
			touch.x += x;
			touch.y += y;
            
			if( touched != NULL ){
				// that container is touched!
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

void mui::Container::handleTouchCanceled( ofTouchEventArgs &touch ){
	touchCanceled(touch);
	if( parent != NULL ){
		parent->handleTouchCanceled(touch);
	}
}



//--------------------------------------------------------------
void mui::Container::reloadTextures(){

}

//--------------------------------------------------------------
void mui::Container::handleReloadTextures(){
	reloadTextures();

	std::vector<Container*>::iterator it = children.begin();
	while( it != children.end() ) {
	    (*it)->handleReloadTextures();
		++it;
	}
}

//--------------------------------------------------------------
bool mui::Container::hasFocus(){
	for( int i = 0; i < OF_MAX_TOUCHES; i++ ){
		if( Root::INSTANCE->respondingContainer[i] == this )
			return true;
	}
	
	return false;
}

//--------------------------------------------------------------
bool mui::Container::hasFocus( ofTouchEventArgs &touch ){
    return Root::INSTANCE->respondingContainer[touch.id] == this;
}

bool mui::Container::requestFocus( ofTouchEventArgs &args ){
	return Root::INSTANCE->becomeResponder(this, args);
}

//--------------------------------------------------------------
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

mui::Container * mui::Container::byName( string name ){
	vector<mui::Container*>::iterator it = children.begin();
	while( it != children.end() ){
		if( (*it)->name == name ) return *it;
		++it;
	}
	
	return NULL;
}

mui::Container * mui::Container::findChildAt( float x, float y, bool onlyVisible ){
	if( x >= 0 && y >= 0 && x <= width && y <= height && (!onlyVisible || visible ) ){
		// do we have a child that fits there?
		vector<mui::Container*>::iterator it = children.begin();

		mui::Container * result = this;
		mui::Container * child = NULL;

		while( it != children.end() ){
			child = (*it)->findChildAt( x - (*it)->x, y - (*it)->y );
			if( child != NULL ) result = child;
			++it;
		}
		
		return result;
	}
	else{
		return NULL;
	}
}

bool mui::Container::isVisibleOnScreen( float border ){
	ofPoint result;
	
	float x = 0;
	float y = 0;
	
	Container * parent = this;
	while( parent != NULL ){
		if( parent->visible == false ) return false;
		
		x += parent->x;
		y += parent->y;
		parent = parent->parent;
	}
	
	result.x = x;
	result.y = y;
	
	ofRectangle me(x-border,y-border,width+2*border,height+2*border);
	ofRectangle root(0,0,MUI_ROOT->width, MUI_ROOT->height );
	
	return me.intersects(root);
}


//--------------------------------------------------------------
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

