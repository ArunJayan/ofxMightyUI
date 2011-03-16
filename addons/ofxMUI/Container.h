/*
 *  Node.h
 *  iPhoneEmptyExample
 *
 *  Created by hansi on 28.01.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MUI_NODE
#define MUI_NODE

namespace mui{
	class Container : public ofxMultiTouchListener{
	public: 
		float x;
		float y;
		float width;
		float height;
		
		ofColor fg;
		ofColor bg;
		
		bool opaque; 
		bool visible; 
		bool ignoreEvents; // false by default. if set to true this thing will never receive events, children of this container however will still receive events. 
		bool singleTouch; // true by default. if set to true a container will remember the first finger touching down and then discard all other events.
		int singleTouchId; // either -1 or the id of the active touch. take this into account for single-touch containers before injecting events (i.e. you shouldn't ever really need this)
		
		string name;
		
		vector<mui::Container*> children;
		Container * parent; 
		
		//bool startedInside[OF_MAX_TOUCHES]; // don't use this. unless you're you really want to. 
		
		Container( float x_, float y_, float width_, float height_ ) : 
		x(x_), y(y_), width(width_), height(height_), opaque(false), parent(NULL), visible(true), ignoreEvents(false), singleTouch(true), name( "" ), singleTouchId( -1 ){
			//for( int i = 0; i < OF_MAX_TOUCHES; i++ ){
			//	startedInside[i] = false; 
			//}
		};
		~Container(){}
		
		void add( Container * c ); 
		
		virtual void update(){};
		virtual void draw(){};
		virtual void drawBackground(); 
		
		virtual void handleDraw(); 
		virtual void handleUpdate();
		
		virtual void touchDown( ofTouchEventArgs &touch ){};
		virtual void touchMoved( ofTouchEventArgs &touch ){};
		virtual void touchMovedOutside( ofTouchEventArgs &touch ){}
		virtual void touchUp( ofTouchEventArgs &touch ){}; 
		virtual void touchUpOutside( ofTouchEventArgs &touch ){}
		virtual void touchDoubleTap( ofTouchEventArgs &touch ){};
		virtual void touchCanceled( ofTouchEventArgs &touch ){}; // when some other component "stole" the responder status. 
		
		virtual Container * handleTouchDown( ofTouchEventArgs &touch );
		virtual Container * handleTouchMoved( ofTouchEventArgs &touch );
		virtual Container * handleTouchUp( ofTouchEventArgs &touch );
		virtual Container * handleTouchDoubleTap( ofTouchEventArgs &touch );
		
		virtual ofPoint getGlobalPosition(); 
		virtual string toString(); 
		
	private: 
	};
};

#endif