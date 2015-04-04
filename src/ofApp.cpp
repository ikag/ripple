#include "ofApp.h"

void ofApp::setup() {
    ofSetVerticalSync(true);

    // handle ESC internally since we use it to exit selection
    ofSetEscapeQuitsApp(false);

    // make sure to load editor font before anything else!
    //ofxEditor::loadFont("fonts/PrintChar21.ttf", 24);
    ofxEditor::loadFont("fonts/topaz/TopazPlus_a1200_v1.0.ttf", 48);
    //ofxEditor::loadFont("fonts/C64_Pro-STYLE.ttf", 48);

    // white text with gray shadow, on black background
    ofBackground(0);
    //editor.getSettings().setTextShadowColor(ofColor::gray);

    // tidal syntax
    tidalSyntax.setWord("$", ofxEditorSyntax::KEYWORD);
    tidalSyntax.setWord("|+|", ofxEditorSyntax::KEYWORD);
    tidalSyntax.setWord("<$>", ofxEditorSyntax::KEYWORD);
    tidalSyntax.setWord("<*>", ofxEditorSyntax::KEYWORD);
    tidalSyntax.setWord("sound", ofxEditorSyntax::FUNCTION);
    tidalSyntax.setSingleLineComment("--");
    tidalSyntax.setMultiLineComment("{-", "-}");
    for (int i = 1; i < 10; i++) {
        tidalSyntax.setWord("d" + ofToString(i), ofxEditorSyntax::KEYWORD);
    }
    editor.getSettings().setLangSyntax("Tidal", &tidalSyntax);

    // tidal highlighter colors
    colorScheme.setStringColor(ofColor::yellow);
    colorScheme.setNumberColor(ofColor::orangeRed);
    colorScheme.setCommentColor(ofColor::gray);
    colorScheme.setKeywordColor(ofColor::fuchsia);
    colorScheme.setTypenameColor(ofColor::red);
    colorScheme.setFunctionColor(ofColor::green);
    editor.setColorScheme(&colorScheme);

    // associate .tidal with Tidal
    editor.getSettings().setFileExtLang("tidal", "Tidal");

    editor.setAutoFocus(true);

    // open a file by default
    ofFile testFile;
    testFile.open("hi.tidal", ofFile::ReadOnly);
    editor.setText(testFile.readToBuffer().getText());

    ofLogNotice() << "num chars: " << editor.getNumCharacters() << " num lines: " << editor.getNumLines();

    debug = false;

    repl.boot("data/boot.hss");
}

void ofApp::draw() {
    editor.draw();

    if (debug) {
        ofSetColor(255);
        ofDrawBitmapString("fps: " + ofToString((int) ofGetFrameRate()), ofGetWidth() - 70, ofGetHeight() - 10);
    }
}

void ofApp::update() {
    repl.read_async();
}

void ofApp::keyPressed(int key) {
    bool modifierPressed = ofxEditor::getSuperAsModifier() ? ofGetKeyPressed(OF_KEY_SUPER) : ofGetKeyPressed(OF_KEY_CONTROL);

    if (modifierPressed) {
        switch (key) {
            case 'q':
                ofExit();
                return;
            case 's':
                if (editor.getColorScheme()) {
                    editor.clearColorScheme();
                } else {
                    editor.setColorScheme(&colorScheme);
                }
                return;
            case 'e':
                executeScript();
                return;
            case 'd':
                debug = !debug;
                return;
            case 'f':
                ofToggleFullscreen();
                return;
            case 'l':
                editor.setLineWrapping(!editor.getLineWrapping());
                return;
            case 'n':
                editor.setLineNumbers(!editor.getLineNumbers());
                return;
            case 'z':
                editor.setAutoFocus(!editor.getAutoFocus());
                return;
            case 'w':
                ofxEditor::setTextShadow(!ofxEditor::getTextShadow());
                return;
            case '1':
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                editor.setCurrentLinePos(1, 5);
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                break;
            case '2':
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                editor.setCurrentLinePos(5, 2);
                ofLogNotice() << "current line: " << editor.getCurrentLine() <<    " pos: " << editor.getCurrentLinePos();
                break;
        }
    }
    editor.keyPressed(key);
}

void ofApp::windowResized(int w, int h) {
    editor.resize(w, h);
}

void ofApp::executeScript() {
    bool selection = editor.isSelection();
    if (selection) {
        editor.flashSelection();
        repl.eval(editor.getText());
    } else {
        repl.eval(":{\n" + getParagraph() + "\n:}");
    }
}

string ofApp::getParagraph() {
    unsigned int pos = editor.getCurrentPos();
    const string text = editor.getText();

    size_t start = text.rfind("\n\n", pos);
    if (start == string::npos) start = 0;

    size_t end = text.find("\n\n", pos);
    if (end == string::npos) end = text.size();

    size_t len = end - start;
    start = start == 0 ? start : (start + 2);

    string subs = text.substr(start, len);
    cerr << "substring: '" << subs << "'" << endl;

    editor.flashText(start, end);

    return subs;
}
