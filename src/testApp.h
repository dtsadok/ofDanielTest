#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxAssimpModelLoader.h"
#include "ofVboMesh.h"
#include "ofx3DModelLoader.h"

//http://innovator.samsungmobile.com/galaxyTab.do
#define TAB_DENSITY 240
#define TAB_XDPI 168
#define TAB_YDPI 168

//IP of Mac
#define HOST_IP "172.26.25.90"
#define PORT 11999
//how long to wait between reconnect attempts (ms)
#define WAIT_TIME 2500

class testApp : public ofBaseApp{

	public:
		void setup();
		void resume();
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
    
		bool bAnimate;
		float animationTime;
        ofxAssimpModelLoader model;

        ofVideoGrabber grabber;

        int zoom, zoomTarget;
        float zoomSpeed;

        ofVboMesh mesh;
        ofPoint position;
        float normScale;
        ofPoint scale;
        ofPoint sceneCenter;
        ofMaterial material;
        ofTexture tex;
        ofLight	light;

        //networking stuff
        //ofxTCPClient tcp;
        ofxTCPServer tcp;
        bool tcpConnected;
        int connectTime;
        ofMatrix4x4 extractMatrix(string msg);

        //comes from network
        ofMatrix4x4 arMatrix;
};

#endif
