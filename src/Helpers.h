/*
 *  Images.h
 *  iPhoneEmptyExample
 *
 *  Created by hansi on 29.01.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MUI_HELPERS
#define MUI_HELPERS

#include "MUI.h"
#include <map>
#include <string>


namespace mui{
	class Helpers{
	public: 
		static ofImage * getImage( string what ); 
		static ofTrueTypeFont * getFont( int size ); 
		static void roundedRect(float x, float y, float w, float h, float r); 
		static void quadraticBezierVertex( float cpx, float cpy, float x, float y, float prevX, float prevY); 
		static void drawStringWithShadow( std::string s, int x, int y, int fontSize, int r, int g, int b ); 
	private: 
		static std::map<std::string, ofImage*> images;
		static std::map<int, ofTrueTypeFont*> fonts;
	}; 
}


#endif
