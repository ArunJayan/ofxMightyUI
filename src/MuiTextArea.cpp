/*
 *  TextArea.cpp
 *
 *  Text TextArea
 *  - Supports vertical and horizontal alignments
 *  - Call commit() after changing text, fontSize, fontName or any of the other variables (except bounds!).
 *  - fg variable affects text color (no commit needed)
 *  - Doesn't support multiline
 */

#include "MuiTextArea.h"
#include "Root.h"

#define STB_TEXTEDIT_STRING mui::TextArea::EditorData
#define STB_TEXTEDIT_CHARTYPE char

#define STB_TEXTEDIT_POSITIONTYPE int
//TODO: utf8 support
#define STB_TEXTEDIT_STRINGLEN(obj) ((int)utf8_count_chars(obj->text))
#define STB_TEXTEDIT_CHARARR_LEN(arr,len) utf8_count_chars(arr)
#define STB_TEXTEDIT_LAYOUTROW(r,obj,n) layout_func(r,obj,n)
#define STB_TEXTEDIT_GETWIDTH(obj,n,i) layout_width(obj,n,i)
#define STB_TEXTEDIT_KEYTOTEXT(key)    (((key) & KEYDOWN_BIT) ? 0 : (key))
// this only returns the first byte of a multi byte sequence :(
#define STB_TEXTEDIT_GETCHAR(tc,i)     ((tc)->text[unicode_idx_to_utf8_idx((tc)->text,i)])
#define STB_TEXTEDIT_NEWLINE           '\n'
#define STB_TEXTEDIT_IS_SPACE(ch)      isspace(ch)
#define STB_TEXTEDIT_DELETECHARS       delete_chars
#define STB_TEXTEDIT_INSERTCHARS       insert_chars

#define KEYDOWN_BIT 0x8000

#define STB_TEXTEDIT_K_SHIFT           0x4000
#define STB_TEXTEDIT_K_CONTROL         0x2000
#define STB_TEXTEDIT_K_LEFT            (KEYDOWN_BIT | 1) // actually use VK_LEFT, SDLK_LEFT, etc
#define STB_TEXTEDIT_K_RIGHT           (KEYDOWN_BIT | 2) // VK_RIGHT
#define STB_TEXTEDIT_K_UP              (KEYDOWN_BIT | 3) // VK_UP
#define STB_TEXTEDIT_K_DOWN            (KEYDOWN_BIT | 4) // VK_DOWN
#define STB_TEXTEDIT_K_LINESTART       (KEYDOWN_BIT | 5) // VK_HOME
#define STB_TEXTEDIT_K_LINEEND         (KEYDOWN_BIT | 6) // VK_END
#define STB_TEXTEDIT_K_TEXTSTART       (STB_TEXTEDIT_K_LINESTART | STB_TEXTEDIT_K_CONTROL)
#define STB_TEXTEDIT_K_TEXTEND         (STB_TEXTEDIT_K_LINEEND   | STB_TEXTEDIT_K_CONTROL)
#define STB_TEXTEDIT_K_DELETE          (KEYDOWN_BIT | 7) // VK_DELETE
#define STB_TEXTEDIT_K_BACKSPACE       (KEYDOWN_BIT | 8) // VK_BACKSPACE
#define STB_TEXTEDIT_K_UNDO            (KEYDOWN_BIT | STB_TEXTEDIT_K_CONTROL | 'z')
#define STB_TEXTEDIT_K_REDO            (KEYDOWN_BIT | STB_TEXTEDIT_K_CONTROL | 'y')
#define STB_TEXTEDIT_K_INSERT          (KEYDOWN_BIT | 9) // VK_INSERT
#define STB_TEXTEDIT_K_WORDLEFT        (STB_TEXTEDIT_K_LEFT  | STB_TEXTEDIT_K_CONTROL)
#define STB_TEXTEDIT_K_WORDRIGHT       (STB_TEXTEDIT_K_RIGHT | STB_TEXTEDIT_K_CONTROL)
#define STB_TEXTEDIT_K_PGUP            (KEYDOWN_BIT | 10) // VK_PGUP -- not implemented
#define STB_TEXTEDIT_K_PGDOWN          (KEYDOWN_BIT | 11) // VK_PGDOWN -- not implemented
#define STB_TEXTEDIT_MOVEWORDLEFT mui_textedit_move_to_word_previous
#define STB_TEXTEDIT_MOVEWORDRIGHT mui_textedit_move_to_word_next

#include "../libs/stb_textedit/include/stb_textedit/stb_textedit.h"

static inline int octect_size( uint32_t codepoint ){
	if (codepoint < 0x80) // two octets
		return 1;
	else if (codepoint < 0x800) // two octets
		return 2;
	else if (codepoint < 0x10000) // three octets
		return 3;
	else // four octets
		return 4;
}

static int utf8_expect_len( char first ){
	switch((unsigned char)first){
		case 0xc0: return 2;
		case 0xe0: return 3;
		case 0xf0: return 4;
		default: return 1;
	}
}

static string unicode_to_utf( uint32_t codepoint ){
	if (codepoint < 0x80)                        // one octet
		return string({(char)codepoint});
	else if (codepoint < 0x800) {                // two octets
		return string({
			(char)((codepoint >> 6)            | 0xc0),
			(char)((codepoint & 0x3f)          | 0x80)
		});
	}
	else if (codepoint < 0x10000) {              // three octets
		return string({
			(char)((codepoint >> 12)           | 0xe0),
			(char)(((codepoint >> 6) & 0x3f)   | 0x80),
			(char)((codepoint & 0x3f)          | 0x80)
		});
	}
	else {
		return string({
			(char)((codepoint >> 18)           | 0xf0),
			(char)(((codepoint >> 12) & 0x3f)  | 0x80),
			(char)(((codepoint >> 6) & 0x3f)   | 0x80),
			(char)((codepoint & 0x3f)          | 0x80),
			(char)((codepoint >> 18)           | 0xf0)
		}); 
	}
}

static inline size_t unicode_idx_to_utf8_idx(const string & str, int unicode_idx ){
	size_t i= 0;
	int utf8_idx = 0;
	
	ofUTF8Iterator oit(str);
	auto it = oit.begin();
	auto end = oit.end();
	while(it != end && unicode_idx != i){
		uint32_t codepoint = *it;
		utf8_idx += octect_size(codepoint);
		++it;
		++i;
	}
	
	return utf8_idx;
}


static inline string utf8_substr(const string & str, int unicode_idx, int unicode_len ){
	if(unicode_len == 0) return "";
	
	size_t i= 0;
	int utf8_idx = 0;
	
	ofUTF8Iterator oit(str);
	auto it = oit.begin();
	auto end = oit.end();
	while(it != end && unicode_idx != i){
		uint32_t codepoint = *it;
		utf8_idx += octect_size(codepoint);
		++it;
		++i;
	}
	
	i = 0;
	int utf8_len = 0;
	while(it != end && unicode_len != i){
		uint32_t codepoint = *it;
		utf8_len += octect_size(codepoint);
		++it;
		++i;
	}
	
	return str.substr(utf8_idx,utf8_len);
}



static inline size_t utf8_count_chars(const string & line ){
	size_t i = 0;
	for( auto c : ofUTF8Iterator(line) ){
		++i;
	}
	return i;
}


// a lot of the stb implementation comes from the example:
// https://github.com/nothings/stb/blob/master/tests/textedit_sample.c
static size_t count_chars( const StyledLine & line ){
	size_t count = 0;
	//TODO:unicodify!
	for( LineElement element : line.elements ){
		count += utf8_count_chars(element.content.styledText.text);
	}
	
	return count;
}

// define the functions we need
static void layout_func(StbTexteditRow *row, mui::TextArea::EditorData *data, int start_i)
{
	const ofRectangle &boundingBox = data->textarea->boundingBox;
	ofRectangle size = mui::Helpers::alignBox( data->textarea, boundingBox.width, boundingBox.height, data->textarea->horizontalAlign, data->textarea->verticalAlign );
//	mui::Helpers::getFontStash().getTextBounds(data->text, data->fontStyle, boundingBox.x, boundingBox.y);

	// figure out where we currently are
	int pos = 0;
	float y = 0;
	for( const StyledLine & line : data->lines ){
		size_t lineLen = count_chars(line);
		
		if( pos >= start_i ){
			assert(line.elements.size()>0);
			row->num_chars = lineLen;
			row->x0 = size.x + line.elements.front().x;
			row->x1 = row->x0 + line.lineW;
			row->baseline_y_delta = 0; //???
			row->ymin = size.y + y;
			row->ymax = size.y + y + line.lineH;
			return;
		}
		
		pos += lineLen;
		y += line.lineH;
	}
	
	row->num_chars = 0;
	row->x0 = size.x;
	row->x1 = size.x+size.width;
	row->baseline_y_delta = 0;
	row->ymin = y;
	row->ymax = y + 10;
	
/*	int remaining_chars = utf8_count_chars(data->text) - start_i;
	//row->num_chars = remaining_chars > 20 ? 20 : remaining_chars; // should do real word wrap here
	row->num_chars = remaining_chars;
	row->x0 = size.x;
	row->x1 = size.x+size.width; // need to account for actual size of characters
	row->baseline_y_delta = size.height;
	row->ymin = size.y;
	row->ymax =  size.y+size.height;*/
}

float layout_width(mui::TextArea::EditorData * data, int n, int i ){
	//TODO: use lines
	int idx = unicode_idx_to_utf8_idx(data->text, n+i);
	int len = utf8_expect_len(data->text[idx]);
	ofRectangle size = mui::Helpers::getFontStash().getTextBounds(data->text.substr(idx,len), data->fontStyle, 0, 0);
	if( i == 0 ){
		return size.x + size.width;
	}
	else{
		int idx2 = unicode_idx_to_utf8_idx(data->text, n+i-1);
		int len2 = len+utf8_expect_len(data->text[idx2]);
		ofRectangle size2 = mui::Helpers::getFontStash().getTextBounds(data->text.substr(idx2,len2), data->fontStyle, 0, 0);
		return size2.width+size2.x - size.width - size.x;
	}
}

// pos is the position in unicode chars
int delete_chars(mui::TextArea::EditorData *data, int pos, int num)
{
	size_t idx = unicode_idx_to_utf8_idx(data->text,pos);
	num = unicode_idx_to_utf8_idx(data->text, pos+num) - idx;
	idx = min(idx,data->text.size());
	num = min(idx+num,data->text.size())-idx;
	if(idx == num) return 0;
	
	data->text.erase(data->text.begin()+idx, data->text.begin()+idx+num);
	data->changed = true;
	return 1;
}

// pos is the position in unicode chars
int insert_chars(mui::TextArea::EditorData *data, int pos, const STB_TEXTEDIT_CHARTYPE *newtext, int num)
{
	size_t idx = unicode_idx_to_utf8_idx(data->text, pos);
	idx = min(data->text.size(), idx);
	data->text.insert(idx, newtext,num);
	data->changed = true;
	return 1; // always succeeds
}


static int mui_is_word_boundary( STB_TEXTEDIT_STRING *str, int idx )
{
	return idx > 0 ? (STB_TEXTEDIT_IS_SPACE( STB_TEXTEDIT_GETCHAR(str,idx-1) ) && !STB_TEXTEDIT_IS_SPACE( STB_TEXTEDIT_GETCHAR(str, idx) ) ) : 1;
}

static bool mui_textedit_is_cool_coding_char(STB_TEXTEDIT_CHARTYPE c){
	const static char * chars = "[]|{}().!=-_,;:";
	for(int i = strlen(chars)-1;i>=0; i--){
		if(chars[i] == c) return true;
	}
	return false;
}

static int mui_textedit_move_to_word_previous( STB_TEXTEDIT_STRING *str, int c )
{
	--c; // always move at least one character
	while( c >= 0 && !mui_is_word_boundary( str, c ) && !mui_textedit_is_cool_coding_char(STB_TEXTEDIT_GETCHAR(str,c)))
		--c;
	
	if( c < 0 )
		c = 0;
	
	return c;
}


static int mui_textedit_move_to_word_next( STB_TEXTEDIT_STRING *str, int c )
{
	const int len = STB_TEXTEDIT_STRINGLEN(str);
	++c; // always move at least one character
	while( c < len && !mui_is_word_boundary( str, c ) && !mui_textedit_is_cool_coding_char(STB_TEXTEDIT_GETCHAR(str,c)))
		++c;
	
	if( c > len )
		c = len;
	
	return c;
}



#define STB_TEXTEDIT_IMPLEMENTATION
#include "../libs/stb_textedit/include/stb_textedit/stb_textedit.h"
#undef STB_TEXTEDIT_IMPLEMENTATION

// this exists only because it's not possible to forward declare unnamed structs
class mui::TextArea::EditorState : public STB_TexteditState{
};

mui::TextArea::TextArea( std::string text_, float x_, float y_, float width_, float height_ ) :
	Container( x_, y_, width_, height_ ),
	text( text_), fontSize(-1), horizontalAlign(Left), verticalAlign(Middle),fontName(""),data(this),lastInteraction(0),selectAllOnFocus(false){
		state = new EditorState();
		focusTransferable = false; 
		stb_textedit_initialize_state(state,0);
		if( fontSize < 0 ) fontSize = mui::MuiConfig::fontSize;
		commit();
};

mui::TextArea::~TextArea(){
	delete state; 
}

//--------------------------------------------------------------
void mui::TextArea::update(){
	data.fontStyle.fontSize = fontSize;
	data.fontStyle.color = fg;
	data.fontStyle.fontID = fontName;
	
	if( data.changed ){
		data.changed = false;
		
		vector<StyledText> blocks{ {data.text,data.fontStyle} };
		data.lines = Helpers::getFontStash().layoutLines(blocks, width);
		data.strlenWithLineStarts = 0;
		bool first = false;
		for( StyledLine line : data.lines ){
			data.strlenWithLineStarts += count_chars(line);
			if( first ) first = false;
			else data.strlenWithLineStarts ++;
		}
		text = data.text;
		ofNotifyEvent(onChange, text, this); 
		commit();
	}
}


//--------------------------------------------------------------
void mui::TextArea::draw(){
	ofRectangle size = Helpers::alignBox( this, boundingBox.width, boundingBox.height, horizontalAlign, verticalAlign );
	
	if( hasKeyboardFocus() && state->select_start != state->select_end ){
		int left = state_select_min();
		int right = state_select_max();
		
		/*ofRectangle bounds1 = mui::Helpers::getFontStash().getTextBounds(data.text.substr(0,left), data.fontStyle, size.x-boundingBox.x, size.y-boundingBox.y);
		ofRectangle bounds2 = mui::Helpers::getFontStash().getTextBounds(data.text.substr(0,right), data.fontStyle, size.x-boundingBox.x, size.y-boundingBox.y);
		*/
		ofSetColor(fg*0.5 + bg*0.5);
		//ofDrawRectangle(bounds1.x+bounds1.width, bounds2.y, bounds2.width-bounds1.width, bounds2.height);
		
		EditorCursor from = getEditorCursorForIndex(left);
		EditorCursor to = getEditorCursorForIndex(right);
		
		ofRectangle rect = from.rect;
		float yy = rect.y;
		
		bool reset = false;
		if( from.lineIt != to.lineIt ){
			// selec to end of line
			rect.width = (*from.lineIt).lineW - rect.x;
			ofDrawRectangle(rect);
			from.lineIt ++;
			reset = true;
			
			yy += (*from.lineIt).lineH;
		}
		
		while( from.lineIt != to.lineIt ){
			StyledLine &line = *from.lineIt;
			float x = size.x-boundingBox.x + line.elements.front().x;
			float y = yy;
			ofDrawRectangle(x, y, line.lineW, line.lineH );
			yy += line.lineH;
			++from.lineIt;
			reset = true;
		}
		
		if( from.lineIt != data.lines.end() ){
			StyledLine &line = *from.lineIt;
			float x = reset?(size.x-boundingBox.x + line.elements.front().x):from.rect.x;
			float y = reset?yy:from.rect.y;
			
			ofDrawRectangle(x, y, to.rect.x + to.rect.width - x, line.lineH );
			++from.lineIt;
		}
	}
	
//	mui::Helpers::getFontStash().drawColumn(data.text, data.fontStyle, size.x-boundingBox.x, size.y-boundingBox.y, width);
	mui::Helpers::getFontStash().drawLines(data.lines, size.x-boundingBox.x, size.y-boundingBox.y, MuiConfig::debugDraw);
	ofSetColor( 255 );
	if( hasKeyboardFocus()){
		ofNoFill();
		ofDrawRectangle(0,0,width,height);
		ofFill();
		// getting the time is slow, but it can only happen
		// for a single textfield here because of the focus (so we're fine)
		uint64_t time = ofGetElapsedTimeMillis();
		if( ((time-lastInteraction)%1000) < 500 ){
			ofRectangle bounds = getEditorCursorForIndex(state->cursor).rect;
			ofDrawRectangle(bounds.x+bounds.width, bounds.y, 2, bounds.height);
		}
	}
}


void mui::TextArea::setText( string text ){
	this->text = text;
	data.text = text;
	data.changed = true;
	update();
}

void mui::TextArea::setTextAndNotify( string text ){
	setText(text);
	ofNotifyEvent(onChange, text, this);
}

//--------------------------------------------------------------
void mui::TextArea::drawBackground(){
    Container::drawBackground(); 
}

//--------------------------------------------------------------
void mui::TextArea::layout(){
	commit();
}

void mui::TextArea::sizeToFit( float padX, float padY ){
	commit(); // update bounding box
	width = boundingBox.width + padX;
	height = boundingBox.height + padY;
	layout(); // tell ourselves about the size change
}

void mui::TextArea::sizeToFitWidth( float padX ){
	commit();
	width = boundingBox.width + padX;
	layout();
}

void mui::TextArea::sizeToFitHeight( float padY ){
	commit();
	height = boundingBox.height + padY;
	layout();
}

//--------------------------------------------------------------
//deprecated
ofRectangle mui::TextArea::box( float t, float r, float b, float l ){
	ofRectangle size = Helpers::alignBox( this, boundingBox.width, boundingBox.height, horizontalAlign, verticalAlign );
	return ofRectangle( size.x - boundingBox.x - l, size.y - boundingBox.y + t, boundingBox.width + l + r, boundingBox.height + t + b );
}

//--------------------------------------------------------------
void mui::TextArea::commit(){
	mui::Helpers::loadFont(fontName);
	boundingBox = Helpers::getFontStash().getTextBounds(text, data.fontStyle, 0, 0);

	data.fontStyle.fontSize = fontSize;
	data.fontStyle.color = fg;
	data.fontStyle.fontID = fontName;
	data.changed = false;
	data.text = text;
	
	vector<StyledText> blocks{ {data.text,data.fontStyle} };
	data.lines = Helpers::getFontStash().layoutLines(blocks, width);
	data.strlenWithLineStarts = 0;
	bool first = false;
	for( StyledLine line : data.lines ){
		data.strlenWithLineStarts += count_chars(line);
		if( first ) first = false;
		else data.strlenWithLineStarts ++;
	}

	// Orient y on a simple uppercase character
	// Otherwise things go up and down unexpectedly
	ofRectangle baselineSize = Helpers::getFontStash().getTextBounds("M", data.fontStyle, 0, 0);
	boundingBox.height = baselineSize.height;
	boundingBox.y = baselineSize.y;
}

void mui::TextArea::touchDown(ofTouchEventArgs &touch){
	lastInteraction = ofGetElapsedTimeMillis();
	if( selectAllOnFocus && !hasKeyboardFocus()){
		stb_textedit_key(&data, state, STB_TEXTEDIT_K_TEXTEND);
		stb_textedit_key(&data, state, STB_TEXTEDIT_K_TEXTSTART | STB_TEXTEDIT_K_SHIFT);
	}
	else{
		stb_textedit_click(&data, state, touch.x, touch.y);
	}
	requestKeyboardFocus();
}

void mui::TextArea::touchMoved(ofTouchEventArgs &touch){
	stb_textedit_drag(&data, state, touch.x, touch.y);
}

bool mui::TextArea::keyPressed( ofKeyEventArgs &key ){
	lastInteraction = ofGetElapsedTimeMillis();
	int keyMask =
		(ofGetKeyPressed(OF_KEY_SHIFT)?STB_TEXTEDIT_K_SHIFT:0) |
		#if defined(TARGET_OSX)
		(ofGetKeyPressed(OF_KEY_ALT)?STB_TEXTEDIT_K_CONTROL:0)
		#else
		(ofGetKeyPressed(OF_KEY_CONTROL)?STB_TEXTEDIT_K_CONTROL:0)
		#endif
	;

#ifdef _WIN32
	// windows is funny with ctrl shortcuts
	cout << key.key << endl;
	if (ofGetKeyPressed(OF_KEY_CONTROL) && key.key >= 1 && key.key <= 26) {
		key.key = key.codepoint = 'a' + (key.key - 1);
	}
	cout << key.key << endl; 
#endif
	
	switch(key.key){
		case OF_KEY_UP:
			// on osx cmd+up goes to the start
			if(ofGetKeyPressed(OF_KEY_COMMAND)) stb_textedit_key(&data, state, STB_TEXTEDIT_K_TEXTSTART);
			else stb_textedit_key(&data, state, STB_TEXTEDIT_K_UP|keyMask);
			break;
		case OF_KEY_DOWN:
			// on osx cmd+down goes to the end
			if(ofGetKeyPressed(OF_KEY_COMMAND)) stb_textedit_key(&data, state, STB_TEXTEDIT_K_TEXTEND);
			else stb_textedit_key(&data, state, STB_TEXTEDIT_K_DOWN|keyMask);
			break;
		case OF_KEY_LEFT:
			if(MUI_ROOT->getKeyPressed(OF_KEY_SUPER)) stb_textedit_key(&data, state, STB_TEXTEDIT_K_LINESTART|keyMask );
			else stb_textedit_key(&data, state, STB_TEXTEDIT_K_LEFT|keyMask);
			break;
		case OF_KEY_RIGHT:
			if(MUI_ROOT->getKeyPressed(OF_KEY_SUPER)) stb_textedit_key(&data, state, STB_TEXTEDIT_K_LINEEND|keyMask );
			else stb_textedit_key(&data, state, STB_TEXTEDIT_K_RIGHT|keyMask);
			break;
		case OF_KEY_BACKSPACE:
			stb_textedit_key(&data, state, STB_TEXTEDIT_K_BACKSPACE|keyMask);
			break;
		case OF_KEY_DEL:
			stb_textedit_key(&data, state, STB_TEXTEDIT_K_DELETE|keyMask);
			break;
		case OF_KEY_RETURN:
			stb_textedit_key(&data, state, STB_TEXTEDIT_NEWLINE|keyMask);
			break;
		case OF_KEY_ESC:
			// do nothing!
			break;
		default:
			//ok, what about other shortcuts? ...
			if(MUI_ROOT->getKeyPressed(MUI_KEY_ACTION) && key.keycode==GLFW_KEY_A ){
				state->select_start = 0;
				state->select_end = data.strlenWithLineStarts;
				state->cursor = state->select_end;
			}
			else if(MUI_ROOT->getKeyPressed(MUI_KEY_ACTION) && key.codepoint == 'z'){
				if (ofGetKeyPressed(OF_KEY_SHIFT)) stb_text_redo(&data, state);
				else stb_text_undo(&data, state);
			}
			else if(MUI_ROOT->getKeyPressed(MUI_KEY_ACTION) && key.codepoint == 'Z'){
				stb_text_redo(&data, state);
			}
			else if(MUI_ROOT->getKeyPressed(MUI_KEY_ACTION) && key.codepoint == 'x'){
				ofGetWindowPtr()->setClipboardString(getSelectedText());
				stb_textedit_key(&data, state, STB_TEXTEDIT_K_DELETE );
			}
			else if(MUI_ROOT->getKeyPressed(MUI_KEY_ACTION) && key.codepoint == 'c'){
				ofGetWindowPtr()->setClipboardString(getSelectedText());
			}
			else if(MUI_ROOT->getKeyPressed(MUI_KEY_ACTION) && key.codepoint == 'v'){
				string text = ofGetWindowPtr()->getClipboardString();
				stb_textedit_paste(&data, state, text.c_str(), text.size());
			}
			else{
				int codept = key.codepoint;
				
				if (!utf8::internal::is_code_point_valid(codept)){
					// what is it? don't know! ignore it!
					return true;
				}
				else if(codept < 0x1F || codept == 0x7F || (codept >= 0x0080 && codept <= 0x009F) ){
					// control character
					return true;
				}
				
				// the next 20 or so lines are basically a verbatim copy from
				// stb_textedit_key() with a tiny change:
				// a char doesn't have a fixed size 
				EditorData * str = &data;
				string insertStr = unicode_to_utf(codept);
				
				// can't add newline in single-line mode
				if (codept == '\n' && state->single_line)
					break;
				
				if (state->insert_mode && !STB_TEXT_HAS_SELECTION(state) && state->cursor < STB_TEXTEDIT_STRINGLEN(str)) {
					stb_text_makeundo_replace(str, state, state->cursor, 1, 1);
					STB_TEXTEDIT_DELETECHARS(str, state->cursor, 1);
					if (STB_TEXTEDIT_INSERTCHARS(str, state->cursor, insertStr.c_str(), insertStr.size())) {
						++state->cursor;
						state->has_preferred_x = 0;
					}
				} else {
					stb_textedit_delete_selection(str,state); // implicity clamps
					if (STB_TEXTEDIT_INSERTCHARS(str, state->cursor, insertStr.c_str(), insertStr.size())) {
						stb_text_makeundo_insert(state, state->cursor, insertStr.size());
						++state->cursor;
						state->has_preferred_x = 0;
					}
				}
			}
	}
	
	return true; 
}

bool mui::TextArea::keyReleased( ofKeyEventArgs &key ){
	return true; 
}


mui::TextArea::EditorCursor mui::TextArea::getEditorCursorForIndex( int cursorPos ){
	EditorCursor result;
	ofRectangle size = mui::Helpers::alignBox( this, boundingBox.width, boundingBox.height, horizontalAlign, verticalAlign );
	
	//TODO: this should be cached
	int pos = 0;
	float yy = 0;
	ofRectangle bounds;
	for( vector<StyledLine>::iterator lineIt = data.lines.begin(); lineIt != data.lines.end(); ++lineIt ){
		StyledLine &line = *lineIt;
		int len = count_chars(line);
		if( pos + len <= cursorPos ){
			pos += len; // +1 for the \n that goes missing
			yy += line.lineH;
		}
		else{
			for( vector<LineElement>::iterator elementIt = line.elements.begin(); elementIt != line.elements.end(); ++elementIt ){
				LineElement &element = *elementIt;
				len = utf8_count_chars(element.content.styledText.text);
				if( pos + len <= cursorPos ){
					pos += len;
				}
				else{
					float x = element.area.x;
					int idx = unicode_idx_to_utf8_idx(element.content.styledText.text, cursorPos - pos);
					bounds = mui::Helpers::getFontStash().getTextBounds(element.content.styledText.text.substr(0,idx), element.content.styledText.style, 0,0);
					bounds.height = line.lineH;
					bounds.x = size.x - boundingBox.x + x + bounds.width;
					bounds.y = size.y - boundingBox.y + yy - line.lineH;
					bounds.width = 0;
					bounds.height = line.lineH;
					
					result.rect = bounds;
					result.lineIt = lineIt;
					result.elementIt = elementIt;
					return result;
				}
			}
			break;
		}
	}
	
	if( bounds == ofRectangle() ){
		// still nothing? must be at last position!
		if( data.lines.size() > 0 ){
			float lineH = data.lines.back().lineH;
			bounds = data.lines.back().elements.back().area;
			bounds.x = size.x - boundingBox.x + bounds.x;
			bounds.y = size.y - boundingBox.y + yy - 2*lineH;
			bounds.height = lineH;
			
			result.lineIt = data.lines.end()-1;
			// there should be always at least one element on each line,
			// even if it's just a SEPARATOR_INVISIBLE
			result.elementIt = data.lines.back().elements.end()-1;
			result.rect = bounds;
		}
		else{
			result.lineIt = data.lines.end();
		}
	}
	
	// this is SLOW!
	// maybe at least remember it?
	return result;
}


int mui::TextArea::state_select_min(){
	return min(state->select_start,state->select_end);
}

int mui::TextArea::state_select_max(){
	return max(state->select_start,state->select_end);
}

int mui::TextArea::state_select_len(){
	int res = state->select_start - state->select_end;
	return res<0?-res:res;
}

string mui::TextArea::getSelectedText(){
	return utf8_substr(data.text, state_select_min(), state_select_len());
}
