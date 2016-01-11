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

#include "MuiCore.h"

namespace mui{
	class Layout;
	
	class Container{
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
		bool focusTransferable; // can the focus travel on to another parent?
		bool allowSubpixelTranslations; // translate by subpixels also? (default)
		bool drawDirty; // force drawing clipped objects in scrollpanes. 
		string name;
		
		vector<mui::Container*> children;
		Container * parent; 
		Layout * layoutHandler;
		
		// put anything you like here...
		void * userData;

		//bool startedInside[OF_MAX_TOUCHES]; // don't use this. unless you're you really want to.
		
		Container( float x_ = 0, float y_ = 0, float width_ = 10, float height_ = 10 );
		
		~Container();
		
		void add( Container * c, int index = -1 ); 
		void remove( Container * c ); 
		void remove();
		
		void setBounds( float x, float y, float width, float height );
        
		virtual void update(){};
		virtual void draw(){};
		virtual void drawBackground(); 
		
		virtual void layout();
		virtual void handleLayout();
		
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
		void handleTouchCanceled( ofTouchEventArgs &touch );

		virtual void reloadTextures();
		virtual void handleReloadTextures();

		virtual bool hasFocus();
		virtual bool hasFocus( ofTouchEventArgs &touch );
		virtual bool requestFocus( ofTouchEventArgs &args );
		virtual ofPoint getGlobalPosition();
		
		mui::Container * byName( string name ); 
		
		// implemented directly in the header because templates ... have some issues
		template <typename T>
		T * findChildOfType( float x, float y, bool onlyVisible = true ){
			mui::Container * thing = findChildAt(x,y,onlyVisible);
			while(thing!=NULL){
				T * result = dynamic_cast<T*>(thing);
				if( result != NULL ){
					return result;
				}
				else{
					thing = thing->parent;
				}
			}
		}

		virtual Container * findChildAt( float x, float y, bool onlyVisible = true );
		
		virtual bool isVisibleOnScreen(float border=0); // effectively visible on screen? border adds an additonal border around, so border<0 means isVisible return false if it's barely visible, border>0 means isVisible will return true even if the component is already slightly off screen.
		virtual string toString();
	private:
	};
};

#endif
