/*
 *  Root.cpp
 *  iPhoneEmptyExample
 *
 *  Created by hansi on 28.01.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Root.h"
#include "ofEventUtils.h"
#include "ofEvents.h"
#include "Container.h"
#include "ScrollPane.h"

using namespace mui;

// TODO: the handleXX functions might return null, even if touchMovedOutside and touchUpOutside 
//       delegated to containers. this shouldn't be the case. 


mui::Root * mui::Root::INSTANCE = NULL;

//--------------------------------------------------------------
mui::Root::Root() : Container( 0, 0, -1, -1 ){
	INSTANCE = this;
	ignoreEvents = true;
	keyboardResponder = NULL;
	init();
};

//--------------------------------------------------------------
void mui::Root::init(){
	#if TARGET_OS_IPHONE
	NativeIOS::init();
//	#elif TARGET_OS_MAC
//	NativeOSX::init();
	#endif
	
	name = "Root"; 
	width = ofGetWidth()/mui::MuiConfig::scaleFactor;
	height = ofGetHeight()/mui::MuiConfig::scaleFactor;
	
	ofAddListener( ofEvents().setup, this, &mui::Root::of_setup, OF_EVENT_ORDER_AFTER_APP );
	ofAddListener( ofEvents().update, this, &mui::Root::of_update, OF_EVENT_ORDER_AFTER_APP );
	ofAddListener( ofEvents().draw, this, &mui::Root::of_draw, OF_EVENT_ORDER_AFTER_APP );
	//ofAddListener( ofEvents().exit, this, &mui::Root::of_exit );
	ofAddListener( ofEvents().windowResized, this, &mui::Root::of_windowResized, OF_EVENT_ORDER_BEFORE_APP );
	ofAddListener( ofEvents().keyPressed, this, &mui::Root::of_keyPressed, OF_EVENT_ORDER_BEFORE_APP );
	ofAddListener( ofEvents().keyReleased, this, &mui::Root::of_keyReleased, OF_EVENT_ORDER_BEFORE_APP );
	ofAddListener( ofEvents().mouseMoved, this, &mui::Root::of_mouseMoved, OF_EVENT_ORDER_BEFORE_APP );
	ofAddListener( ofEvents().mouseDragged, this, &mui::Root::of_mouseDragged, OF_EVENT_ORDER_BEFORE_APP );
	ofAddListener( ofEvents().mousePressed, this, &mui::Root::of_mousePressed, OF_EVENT_ORDER_BEFORE_APP );
	ofAddListener( ofEvents().mouseReleased, this, &mui::Root::of_mouseReleased, OF_EVENT_ORDER_BEFORE_APP );
	ofAddListener( ofEvents().mouseScrolled, this, &mui::Root::of_mouseScrolled, OF_EVENT_ORDER_BEFORE_APP );
	ofAddListener( ofEvents().touchDown, this, &mui::Root::of_touchDown, OF_EVENT_ORDER_BEFORE_APP );
	ofAddListener( ofEvents().touchUp, this, &mui::Root::of_touchUp, OF_EVENT_ORDER_BEFORE_APP );
	ofAddListener( ofEvents().touchMoved, this, &mui::Root::of_touchMoved, OF_EVENT_ORDER_BEFORE_APP );
	ofAddListener( ofEvents().touchDoubleTap, this, &mui::Root::of_touchDoubleTap, OF_EVENT_ORDER_BEFORE_APP );
	ofAddListener( ofEvents().touchCancelled, this, &mui::Root::of_touchCancelled, OF_EVENT_ORDER_BEFORE_APP );
	//ofAddListener( ofEvents().messageEvent, this, &mui::Root::of_messageEvent );
	//ofAddListener( ofEvents().fileDragEvent, this, &mui::Root::of_fileDragEvent );
	
	
	// this seems unclear ... let's better put this in place!
	for( int i = 0; i < OF_MAX_TOUCHES; i++ ){
		touchResponder[i] = NULL;
	}
}

void mui::Root::handleUpdate(){
	int _width = ofGetWidth()/mui::MuiConfig::scaleFactor;
	int _height = ofGetHeight()/mui::MuiConfig::scaleFactor;

	if( width != _width || height != _height ){
		width = _width;
		height = _height;
		handleLayout();
	}
	else if( ofGetFrameNum() == 0 ){
		handleLayout();
	}
	
	tweener.step( ofGetSystemTime() );
	#if TARGET_OS_IPHONE
	NativeIOS::update();
//	#elif TARGET_OS_MAC
//	NativeOSX::init();
	#endif
	Container::handleUpdate();
    
    handleRemovals();
}

//--------------------------------------------------------------
void mui::Root::handleDraw(){
	ofSetupScreenOrtho(); 
	ofPushStyle();
	ofScale( mui::MuiConfig::scaleFactor, mui::MuiConfig::scaleFactor, mui::MuiConfig::scaleFactor );
	ofFill(); 
	ofSetLineWidth( 1 ); 
	ofSetColor( 255, 255, 255 ); 
	ofEnableAlphaBlending();
	
	Container::handleDraw();
	
    handleRemovals();
	
	if( mui::MuiConfig::debugDraw ){
		mui::Container * active = this->findChildAt( ofGetMouseX()/mui::MuiConfig::scaleFactor - this->x, ofGetMouseY()/mui::MuiConfig::scaleFactor-this->y, true );
		if( active != NULL ){
			ofPoint p = active->getGlobalPosition();
			ofPushMatrix();
			ofFill();
			string name;
			mui::Container * c = active;
			string size;
			while( c != NULL  ){
				name = c->name + (name==""?"":">") + name;
				c = c->parent;
			}
			
			ofRectangle b = active->getGlobalBounds();
			size = (stringstream() << "Pos:" << b.x << "," << b.y << "  " << b.width << " x " << b.height).str();
			
			
			ofxFontStashStyle style = mui::Helpers::getStyle(10);
			ofRectangle nameB = mui::Helpers::getFontStash().getTextBounds(name, style, p.x, p.y+10);
			ofRectangle sizeB = mui::Helpers::getFontStash().getTextBounds(size, style, p.x, p.y+20);
			ofDrawRectangle( nameB );
			ofDrawRectangle( sizeB );
			ofNoFill();
			ofSetColor( 255,255,0 );
			ofDrawRectangle( p.x, p.y, active->width, active->height );
			ofSetColor(255);
			ofFill();
			mui::Helpers::getFontStash().draw(name, style, p.x, p.y+10);
			mui::Helpers::getFontStash().draw(size, style, p.x, p.y+20);
			ofPopMatrix();
		}
	}
	
	ofPopStyle();
}



//--------------------------------------------------------------
mui::Container * mui::Root::handleTouchDown( ofTouchEventArgs &touch ){
	#if TARGET_OS_IPHONE
	NativeIOS::hide();
//	#elif TARGET_OS_MAC
//	NativeOSX::hide();
	#endif
	
	ofTouchEventArgs copy = touch; 
	fixTouchPosition( touch, copy, NULL ); 
	
	//return ( touchResponder[touch.id] = Container::handleTouchDown( copy ) ); 
	touchResponder[touch.id] = Container::handleTouchDown( copy );
	if( touchResponder[touch.id] != keyboardResponder ) keyboardResponder = NULL;
	
	return touchResponder[touch.id]; 
}


//--------------------------------------------------------------
mui::Container * mui::Root::handleTouchMoved( ofTouchEventArgs &touch ){
	ofTouchEventArgs copy = touch; 
	fixTouchPosition( touch, copy, NULL ); 
	Container * touched = Container::handleTouchMoved( copy );

	if( touched != touchResponder[touch.id] && touchResponder[touch.id] != NULL ){
		copy = touch;
		fixTouchPosition( touch, copy, NULL );
        copy = Helpers::translateTouch( copy, this, touchResponder[touch.id] );
        touchResponder[touch.id]->touchMovedOutside( copy );
		return touchResponder[touch.id];
	}
	
	return touched;
}


//--------------------------------------------------------------
mui::Container * mui::Root::handleTouchUp( ofTouchEventArgs &touch ){
	ofTouchEventArgs copy = touch; 
	fixTouchPosition( touch, copy, NULL ); 
	Container * touched = Container::handleTouchUp( copy ); 
	
	if( touched != touchResponder[touch.id] && touchResponder[touch.id] != NULL ){
		fixTouchPosition( touch, copy, touchResponder[touch.id] );
		Container *c = touchResponder[touch.id];
		touchResponder[touch.id]->touchUpOutside( copy );
		c->singleTouchId = -1;
	}
	
    touchResponder[touch.id] = NULL; 
    
	return touched; 	
}


//--------------------------------------------------------------
mui::Container * mui::Root::handleTouchDoubleTap( ofTouchEventArgs &touch ){
	ofTouchEventArgs copy = touch; 
	fixTouchPosition( touch, copy, NULL ); 

	return Container::handleTouchDoubleTap( copy ); 
}

//--------------------------------------------------------------
mui::Container * mui::Root::handleTouchCancelled( ofTouchEventArgs &touch ){
	if( touchResponder[touch.id] != NULL ){
		touchResponder[touch.id]->touchCanceled( touch );
		touchResponder[touch.id]->singleTouchId = -1;
		mui::Container * c = touchResponder[touch.id];
		touchResponder[touch.id] = NULL;
		return c;
	}
	else{
		return NULL;
	}
}



//--------------------------------------------------------------
void mui::Root::fixTouchPosition( ofVec2f &touch, ofVec2f &copy, Container * container ){
	copy.x = touch.x/mui::MuiConfig::scaleFactor;
	copy.y = touch.y/mui::MuiConfig::scaleFactor;
	
	if( container != NULL ){
		ofPoint pos = container->getGlobalPosition();
		copy.x -= pos.x;
		copy.y -= pos.y;
	}
}




//--------------------------------------------------------------
void mui::Root::showTextField( TextField * tf ){
	#if TARGET_OS_IPHONE
	NativeIOS::showTextField( tf );
//	#elif TARGET_OS_MAC
//	NativeOSX::showTextField( tf );
	#endif
}

void mui::Root::hideTextFields(){
    #if TARGET_OS_IPHONE
    NativeIOS::hide();
//	#elif TARGET_OS_MAC
//	NativeOSX::hide();
    #endif
}


//--------------------------------------------------------------
bool mui::Root::becomeTouchResponder( Container * c, ofTouchEventArgs &touch ){
	// the trivial case ...
	if( c != NULL && c == touchResponder[touch.id] )
		return true;
	
	// notify previous owner,
	// cancel if it doesn't allow transfering focus
	if( touchResponder[touch.id] != NULL ){
        if( touchResponder[touch.id]->focusTransferable == false )
            return false; 
        
		touchResponder[touch.id]->handleTouchCanceled( touch );
		touchResponder[touch.id]->singleTouchId = -1; 
	}
	
	// alright, install new owner
	touchResponder[touch.id] = c;
	if( touchResponder[touch.id] != NULL ){
		touchResponder[touch.id]->singleTouchId = touch.id;
	}

	
	return true; 
}

bool mui::Root::becomeKeyboardResponder( Container * c ){
	this->keyboardResponder = c;
	return true;
}


//--------------------------------------------------------------
void mui::Root::safeRemove( Container * c ){
    safeRemoveList.push_back( c ); 
}

//--------------------------------------------------------------
void mui::Root::safeRemoveAndDelete( mui::Container *c ){
    safeRemoveAndDeleteList.push_back( c ); 
}

//--------------------------------------------------------------
void mui::Root::removeFromResponders( Container * c ){
	for( int i = 0; i < OF_MAX_TOUCHES; i++ ){
		if( touchResponder[i] == c ){
			touchResponder[i] = NULL;
		}
	}
	if( keyboardResponder == c ){
		keyboardResponder = NULL;
	}
}

void mui::Root::reloadTextures(){
	mui::Helpers::clearCaches();
}



//--------------------------------------------------------------
void mui::Root::prepareAnimation( int milliseconds, int type, int direction ){
	param = tween::TweenerParam( milliseconds, (short)type, (short)direction );
}

bool mui::Root::getKeyPressed( int key ){
	return activeKeys.find(key) != activeKeys.end();
}

//--------------------------------------------------------------
void mui::Root::animate( float &variable, float targetValue ){
    param.addProperty( &variable, targetValue ); 
}


//--------------------------------------------------------------
void mui::Root::commitAnimation(){
    tweener.addTween( param );
}

void mui::Root::handleRemovals(){
    vector<Container*>::iterator it = safeRemoveList.begin(); 
    while( it != safeRemoveList.end() ){
        (*it)->remove();
        ++it; 
    }
    
    it = safeRemoveAndDeleteList.begin(); 
    while( it != safeRemoveAndDeleteList.end() ){
        (*it)->remove();
        delete (*it); 
        ++it; 
    }
    
    safeRemoveList.clear(); 
    safeRemoveAndDeleteList.clear(); 
}

//--------------------------------------------------------------
mui::Container * mui::Root::handleKeyPressed( ofKeyEventArgs &event ){
	// this is a bit awkward, there seems to be a bug in
	// glfw that re-sends modifier key when they are released.
	// for now, i'm not fixing this. i really shouldn't be, not here.
	activeKeys.insert(event.key);
	if( keyboardResponder != NULL ){
		keyboardResponder->keyPressed(event);
	}
	
	return keyboardResponder;
}

//--------------------------------------------------------------
mui::Container * mui::Root::handleKeyReleased( ofKeyEventArgs &event ){
	set<int>::iterator it = activeKeys.find(event.key);
	if( it != activeKeys.end() ){
		activeKeys.erase(event.key);
	}
	if( keyboardResponder != NULL ){
		keyboardResponder->keyReleased(event);
	}
	
	return keyboardResponder;
}

//--------------------------------------------------------------
mui::Container * mui::Root::handleMouseMoved( float x, float y ){
	ofTouchEventArgs args;
	args.x = x;
	args.y = y;
	args.id = 0;
	return handleTouchMoved(args);
}

//--------------------------------------------------------------
mui::Container * mui::Root::handleMouseDragged( float x, float y, int button ){
	ofTouchEventArgs args;
	args.x = x;
	args.y = y;
	args.id = 0;
	return handleTouchMoved(args);
}

//--------------------------------------------------------------
mui::Container * mui::Root::handleMousePressed( float x, float y, int button ){
	ofTouchEventArgs args;
	args.x = x;
	args.y = y;
	args.id = 0;
	return handleTouchDown(args);
}

//--------------------------------------------------------------
mui::Container * mui::Root::handleMouseReleased( float x, float y, int button ){
	ofTouchEventArgs args;
	args.x = x;
	args.y = y;
	args.id = 0;
	return handleTouchUp(args);
}



void mui::Root::of_setup( ofEventArgs &args ){
	//handleSetup();
	handleLayout(); 
}
void mui::Root::of_update( ofEventArgs &args ){
	handleUpdate();
}
void mui::Root::of_draw( ofEventArgs &args ){
	handleDraw();
}
void mui::Root::of_exit( ofEventArgs &args ){
	//handleExit(args);
}
void mui::Root::of_windowResized( ofResizeEventArgs &args ){
	//handleWindowResized(args);
	width = args.width/mui::MuiConfig::scaleFactor;
	height = args.height/mui::MuiConfig::scaleFactor;
	handleLayout();
}
bool mui::Root::of_keyPressed( ofKeyEventArgs &args ){
	return handleKeyPressed(args) != NULL;
}
bool mui::Root::of_keyReleased( ofKeyEventArgs &args ){
	return handleKeyReleased(args) != NULL;
}
bool mui::Root::of_mouseMoved( ofMouseEventArgs &args ){
	return handleMouseMoved(args.x, args.y) != NULL;
}
bool mui::Root::of_mouseDragged( ofMouseEventArgs &args ){
	return handleMouseDragged(args.x, args.y, args.button) != NULL;
}
bool mui::Root::of_mousePressed( ofMouseEventArgs &args ){
	return handleMousePressed(args.x, args.y, args.button) != NULL;
}
bool mui::Root::of_mouseReleased( ofMouseEventArgs &args ){
	return handleMouseReleased(args.x, args.y, args.button) != NULL;
}
bool mui::Root::of_mouseScrolled( ofMouseEventArgs &args ){
	ofVec2f pos;
	fixTouchPosition(args, pos, NULL);
	mui::Container * container = (mui::Container*)findChildOfType<mui::ScrollPane>(pos.x, pos.y);
	if( container != NULL ){
		container->mouseScroll(args);
		return true;
	}
	else{
		return false;
	}
}
bool mui::Root::of_touchDown( ofTouchEventArgs &args ){
	return handleTouchDown(args) != NULL;
}
bool mui::Root::of_touchUp( ofTouchEventArgs &args ){
	return handleTouchUp(args) != NULL;
}
bool mui::Root::of_touchMoved( ofTouchEventArgs &args ){
	return handleTouchMoved(args) != NULL;
}
bool mui::Root::of_touchDoubleTap( ofTouchEventArgs &args ){
	return handleTouchDoubleTap(args) != NULL;
}
bool mui::Root::of_touchCancelled( ofTouchEventArgs &args ){
	return handleTouchCancelled(args) != NULL;
}
void mui::Root::of_messageEvent( ofMessage &args ){
	//handleMessageEvent(args);
}
void mui::Root::of_fileDragEvent( ofDragInfo &args ){
	//handleFileDragEvent(args);
}