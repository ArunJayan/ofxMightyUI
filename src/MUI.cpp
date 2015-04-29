/*
 *  MUI.cpp
 *  iPhoneEmptyExample
 *
 *  Created by hansi on 28.01.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "MUI.h"
#include <Poco/Util/Application.h>

#if TARGET_OS_IPHONE
	#include "ofAppiOSWindow.h"
	#include <CoreFoundation/CoreFoundation.h>
#elif TARGET_OS_MAC
	#include "ofAppGLFWWindow.h"
#endif

void mui_init(){
	#if TARGET_OS_IPHONE
	if( mui::MuiConfig::detectRetina ){
		ofAppiOSWindow * w = ofAppiOSWindow::getInstance();
		if( w->isRetinaEnabled() ){
			mui::MuiConfig::scaleFactor = 2;
			mui::MuiConfig::useRetinaAssets = true;
		}
	}
	#endif
	//TODO: allow retina in osx too!
	
	Poco::Path appPath;
	#if TARGET_OS_IPHONE
		// http://www.cocoabuilder.com/archive/cocoa/193451-finding-out-executable-location-from-c-program.html
		CFBundleRef bundle = CFBundleGetMainBundle();
		CFURLRef    url  = CFBundleCopyExecutableURL(bundle); // CFBundleCopyResourcesDirectoryURL(bundle);
		CFURLRef absolute = CFURLCopyAbsoluteURL(url);
		CFStringRef path  = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
		CFIndex    maxLength = CFStringGetMaximumSizeOfFileSystemRepresentation(path);
		char        *result = (char*)malloc(maxLength);
		
		if(result) {
			if(!CFStringGetFileSystemRepresentation(path,result, maxLength)) {
				free(result);
				result = NULL;
			}
		}
		
		CFRelease(path);
		CFRelease(url);
		CFRelease(absolute);
		appPath = Poco::Path(result);
		appPath = appPath.parent();
	#elif TARGET_OS_MAC
		// http://www.cocoabuilder.com/archive/cocoa/193451-finding-out-executable-location-from-c-program.html
		CFBundleRef bundle = CFBundleGetMainBundle();
		CFURLRef    url  = CFBundleCopyExecutableURL(bundle); // CFBundleCopyResourcesDirectoryURL(bundle);
		CFURLRef absolute = CFURLCopyAbsoluteURL(url);
		CFStringRef path  = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
		CFIndex    maxLength = CFStringGetMaximumSizeOfFileSystemRepresentation(path);
		char        *result = (char*)malloc(maxLength);
		
		if(result) {
			if(!CFStringGetFileSystemRepresentation(path,result, maxLength)) {
				free(result);
				result = NULL;
			}
		}
		
		CFRelease(path);
		CFRelease(url);
		CFRelease(absolute);
		appPath = Poco::Path(result);
		appPath = appPath.parent().parent().pushDirectory("Resources");
	
		if( mui::MuiConfig::detectRetina ){
			ofAppGLFWWindow * window = (ofAppGLFWWindow*)ofGetWindowPtr();
			if( window != NULL ){
				mui::MuiConfig::scaleFactor = window->getPixelScreenCoordScale();
			}
		}
	#else
		appPath = Poco::Path(ofToDataPath("", true));
	#endif
	
	mui::MuiConfig::dataPath = appPath.absolute();
}


string mui::MuiConfig::font = "mui/fonts/Lato-Regular.ttf";
bool mui::MuiConfig::debugDraw = false;
int mui::MuiConfig::scrollPaneBleed = 30;
int mui::MuiConfig::scrollToBaseDuration = 600;
int mui::MuiConfig::scrollVelocityDecrease = 300;
int mui::MuiConfig::fontSize = 12;
bool mui::MuiConfig::detectRetina = true;
bool mui::MuiConfig::useRetinaAssets = true;
int mui::MuiConfig::scaleFactor = 1;
Poco::Path mui::MuiConfig::dataPath = Poco::Path();
ofLogLevel mui::MuiConfig::logLevel = OF_LOG_ERROR; 