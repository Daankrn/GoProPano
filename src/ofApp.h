#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxHttpUtils.h"
#include "ofxGumbo.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void goProCommand(string device, string cmd);
        void goProCommand(string device, string cmd, string option);
        void urlResponse(ofHttpResponse & response);
    void search_for_links(GumboNode* node);
    void getLinks();
    void getImages();
    void startStich(int pano);
    void inPosition();
    
    stringstream url;
    ofxHttpUtils http;
    string password = "GOPROHERO";
    string bacpac = "bacpac";
    string camera = "camera";
    
    
    float startTime;
    float timer;
    int id;
    
    int panoNum;
    int panoNumChk;
    int imgNum;
    int imgNumChk;
    
    bool downloadedPicture;
    
    ofFile pts;
    ofBuffer bashBuffer;
    ofFile bashFile;
    
    string goProFiles = "http://10.5.5.9:8080/videos/DCIM/100GOPRO/";
    
    
    vector<string> links;
};
