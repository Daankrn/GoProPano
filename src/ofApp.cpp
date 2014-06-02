#include "ofApp.h"


bool panoCheck[9999][8];

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofRegisterURLNotification(this);
    timer = startTime = ofGetElapsedTimeMillis();
    http.start();
    start = false;

    downloadedPicture = true;
    panoNum = panoNumChk = imgNum =imgNumChk = 0;
    pts.open(ofToDataPath("stitch/pts.pts"), ofFile::ReadWrite, false);
    //turn on GoPro
    goProCommand(bacpac, "PW", "01");
    
    simulDown = picSum = totalPicSum = picsDownloaded = 0;
    tempStop = false;
    avrPanoTime = 0;

}

//--------------------------------------------------------------
void ofApp::update(){
    // timer if it's been 1sec after command to take picture.
    timer = ofGetElapsedTimeMillis() - startTime;
    if(!downloadedPicture && timer > 1000){
        downloadedPicture = true;
        getLinks();
        getImages();
    }
    //TEMP TEMP TEMP TEMP TEMP REPLACE WITH ARDUINO//
    if(start && !tempStop && timer > 3000){
        //take new picture.
        inPosition();
    }
    
    
    //calculate total time
    seconds = ofGetElapsedTimeMillis()/1000%60;
    minutes = ofGetElapsedTimeMillis()/1000/60;
    if(panoNumChk > 0){
        avrPanoTime = panoNumChk/minutes;
    }
}



//--------------------------------------------------------------
//TODO: add some feedback on status of GoPro
//      picture download cue
//      picture download statusbars
void ofApp::draw(){
    ofDrawBitmapString("# of downloads = "+ofToString(simulDown), 15, 30);
    ofDrawBitmapString("    total pics = "+ofToString(totalPicSum), 15, 45);
    ofDrawBitmapString("  current pano = "+ofToString(panoNumChk), 15, 60);
    ofDrawBitmapString("    total time = "+ofToString(minutes)+"m"+ofToString(seconds)+"s", 15, 90);
    ofDrawBitmapString("  panos/minute = "+ofToString(avrPanoTime), 15, 105);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'p':
            goProCommand("bacpac", "SH", "01");
            downloadedPicture = false;
            startTime = ofGetElapsedTimeMillis();
            break;
        case 'g':
            getLinks();
            break;
        case 'd':
            getImages();
            break;
        case 'i': initGoPro(); break;
        case 's':start = true;break;
        default:
            break;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::goProCommand(string device, string cmd){
    http.addUrl("http://10.5.5.9/"+device+"/"+cmd+"?t="+password);
}

//--------------------------------------------------------------
//check link below for all possible commands
//https://github.com/KonradIT/goprowifihack/blob/master/WiFi-Commands.mkdn
//credit to KonradIT
void ofApp::goProCommand(string device, string cmd, string option){
    http.addUrl("http://10.5.5.9/"+device+"/"+cmd+"?t="+password+"&p=%"+option);
    // sends command to gopro
    ofLogNotice("http://10.5.5.9/"+device+"/"+cmd+"?t="+password+"&p=%"+option);
}




//--------------------------------------------------------------
//function that should be called by Arduino
//it's a signal that it's safe to take a picture
void ofApp::inPosition(){
    goProCommand("bacpac", "SH", "01");             //send command to gopro
    downloadedPicture = false;                      //declare there's a new pic
    startTime = ofGetElapsedTimeMillis();//start picture timer
}


//--------------------------------------------------------------
void ofApp::getImages(){
    // get last image
    cout << "downloading " << links.back() << endl;
    // save to file in correct folder
    request = ofSaveURLAsync(goProFiles+links.back(),ofToString(panoNum)+"/img/"+ofToString(imgNum)+".jpg");
    
    picSum++;
    simulDown++; //calculate simultaneous downloads
    imgNum++;
    //simple wrap around counter
    if(imgNum > 7) {
        imgNum = 00;
        panoNum++;
    }
    //pause image taking if 28 panorama's are taken
    if (picSum == 195) {
        tempStop = true;
    }
}

// start stitching!
void ofApp::startStich(int pano){
    //save images in one folder, same name
    ofLogNotice("starting Stitch");
    
    //set bash file
    bashBuffer.set("#!/bin/bash \n open '/Applications/PTGui Pro.app' -n -W --args -batch -x '/Users/daankrijnen/Programming/of_v0.8.1_osx_release/apps/myApps/GoProPano/bin/data/"+ofToString(pano)+"/"+ofToString(pano)+"_R.pts'");
    ofBufferToFile("stitch.sh", bashBuffer); // save as .sh
    bashFile.open("stitch.sh");
    bashFile.setExecutable(true);       // set as executable
    string bashPath;
    bashPath = bashFile.path();
    
    //copy pts project file
    pts.copyTo(ofToDataPath(ofToString(pano)+"/"+ofToString(pano)+"_R.pts"));
    
    char *pathChar;
    pathChar = new char[bashPath.length()+1];
    strcpy(pathChar, bashPath.c_str());
    int pid = fork();                   //create new process
    cout << "pid::" << pid << endl;
    if(pid==0){
        execl(pathChar, pathChar, NULL); //execute process, opens ptgui pro.
    }
    else{
        cout << "failed PID::" << pid << endl;
    }
    bashBuffer.clear();
    bashFile.close();
    return;
}

void ofApp::getLinks(){
    links.clear();                                                  //delete old links
    ofSaveURLTo(goProFiles,"files.html");                           //save new links
    ofBuffer buffer = ofBufferFromFile("files.html");
    GumboOutput* output = gumbo_parse(buffer.getText().c_str());    //parse text
    search_for_links(output->root);                                 //extract links
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    links.erase(links.begin(), links.begin()+2);                    //delete first non-file entries
}

//--------------------------------------------------------------
void ofApp::urlResponse(ofHttpResponse & response){
    if(response.status==200){
        cout << response.request.name << " downloaded" << endl;
        cout << "pano=" << panoNumChk << " img=" << imgNumChk << endl;
        imgNumChk++; //count for within panorama
        picsDownloaded++; //count for deleting all files in time
        totalPicSum++; //count for total pics
        simulDown--; //picture done downloading, delete 1.
        if(imgNumChk > 7){
            startStich(panoNumChk);
            imgNumChk = 0;
            panoNumChk++;
        }
        //if all images in queue are downloaded
        if(picsDownloaded == picSum){
            goProCommand(camera, "DA");     //delete all pics
            picsDownloaded = 0;
            picSum = 0;
            tempStop = false;               //start taking new pics
        }
        //reply if file downloaded
    }else{
		cout << response.status << " " << response.error << endl;
	}
}


//--------------------------------------------------------------
void ofApp::search_for_links(GumboNode* node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }
    
    GumboAttribute* href;
    
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href")))
    {
        links.push_back(href->value);
        //std::cout << href->value << std::endl;
        
    }
    
    GumboVector* children = &node->v.element.children;
    
    for (unsigned int i = 0; i < children->length; ++i)
    {
        search_for_links(static_cast<GumboNode*>(children->data[i]));
        
    }
}

//--------------------------------------------------------------
void ofApp::initGoPro(){
    //set to correct mode
    goProCommand(camera, "CM", "01");
    ofSleepMillis(300);
    //silence gopro
    goProCommand(camera, "BS", "00");
    //ofSleepMillis(300);
    //delete all images
    //goProCommand(camera, "DA");
    ofSleepMillis(300);
    //set resolution to 12mp wide
    //goProCommand(camera, "PR", "05");
    //turn off LEDs
    //goProCommand(bacpac, "LB", "01");
    ofLogNotice("initiated");
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
