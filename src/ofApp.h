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
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        //send commands to gopro
        void goProCommand(string device, string cmd);
        void goProCommand(string device, string cmd, string option);
    
        //response by HTTP
        void urlResponse(ofHttpResponse & response);

    void inPosition();                      //to be called by arduino, takes pictures MAIN FUNCTION
    void initGoPro();                       //initialize GoPro, set all values correct
    void search_for_links(GumboNode* node);
    void getLinks();                        //gets HTML file list from GoPro
    void getImages();                       //downloads images from GoPro
    void startStich(int pano);              //initiates panorama

    
    ofxHttpUtils http;                      //used to talk to GoPro
    string password = "GOPROHERO";          //set password
    string bacpac = "bacpac";               //standard stuff
    string camera = "camera";
    
    
    float startTime;                        //time when pic is taken
    float timer;                            //timer since startime
    int request;                            //request ID for downloads
    
    int panoNum;                            //last pano finished taking pictures
    int panoNumChk;                         //last pano finished downloading pictures
    int imgNum;                             //last image taken in panorama sequence (0-7)
    int imgNumChk;                          //last image downloaded in panorama sequence (0-7)
    
    bool downloadedPicture;                 //make sure download request is done once
    bool start;                             //main start bool
    bool tempStop;                          //temporary stop for deleting pictures
    
    float simulDown;                        //number of downloads running at once
    float picSum;                           //number of pictures taken since last delete
    float totalPicSum;                      //number of pictures taken since run
    float picsDownloaded;                   //number of pictures downloaded since delete
    
    float seconds;
    float minutes;
    float avrPanoTime;                      //average time in minutes to complete 1 panorama (based on downloads)
    
    ofFile pts;                             //PTGui Pro stitch project file
    ofBuffer bashBuffer;                    //buffer for script to run stitching
    ofFile bashFile;                        //file for script to run stitching
    
    string goProFiles = "http://10.5.5.9:8080/videos/DCIM/100GOPRO/"; //link to source file folder of GoPro
    
    
    vector<string> links;                   //vector of links to GoPro pictures
};
