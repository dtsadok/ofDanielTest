#include "testApp.h"

string debugMessage;

//--------------------------------------------------------------
void testApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetOrientation(OF_ORIENTATION_90_LEFT);

    //zoom in effect
    zoom       = -500;
    zoomTarget = 350;
    zoomSpeed = 0.4;

    grabber.initGrabber(800, 600);
	
    // we need GL_TEXTURE_2D for our models coords.
    ofDisableArbTex();

    if (model.loadModel("astroBoy_walk.dae", true))
    {
    	model.setAnimation(0);
    	model.setPosition(ofGetWidth()/2, (float)ofGetHeight() * 0.75 , 0);
    	//model.createLightsFromAiModel();
    	//model.disableTextures();
    	//model.disableMaterials();

    	mesh = model.getMesh(0);
    	position = model.getPosition();
    	normScale = model.getNormalizedScale();
    	scale = model.getScale();
    	sceneCenter = model.getSceneCenter();
    	material = model.getMaterialForMesh(0);
        tex = model.getTextureForMesh(0);
    }

    ofEnableBlendMode(OF_BLENDMODE_ALPHA);

	glEnable(GL_DEPTH_TEST);

    //some model / light stuff
    glShadeModel(GL_SMOOTH);
    light.setup();
    ofEnableSeparateSpecularLight();

	
	bAnimate		= false;
	animationTime	= 0.0;

	tcpConnected = tcp.setup(HOST_IP, PORT, false); //nonblocking
	if (tcpConnected) tcp.sendRaw("Phew!\n");

	drawCamera = false;

	debugMessage = "Bupkes...";
}

void testApp::resume()
{
}

//--------------------------------------------------------------
void testApp::update(){
	grabber.update();

    //sweet - this is how you zoom in
    zoom += (zoomTarget-zoom) * zoomSpeed;

	//this is for animation if the model has it. 
	if( bAnimate ){
		animationTime += ofGetLastFrameTime();
		if( animationTime >= 1.0 ){
			animationTime = 0.0;
		}
	    model.setNormalizedTime(animationTime);
		mesh = model.getCurrentAnimatedMesh(0);
	}

	if (tcpConnected)
	{
		//call/response (?)
		//tcp.send("A");
		//sleep?

		string msg = tcp.receive();
		if( msg.length() > 0 )
		{
			debugMessage = "--( " + msg + " ) --";
			arMatrix = extractMatrix(msg);
			smoothARMatrix();
		}
	}
	else //try to reconnect
	{
		if  (ofGetElapsedTimeMillis() - connectTime > WAIT_TIME)
		{
			//ofLog("Trying to connect to host...");
			tcpConnected = tcp.setup(HOST_IP, PORT);
			connectTime = ofGetElapsedTimeMillis();
			if (tcpConnected) tcp.send("Phew!");
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
    //ofBackground(50, 50, 50, 0);
	ofBackground(0, 0, 0);
    ofSetColor(255, 255, 255, 255);

    if (drawCamera)
    	grabber.draw((1024-800)/2, 0);

    ofPushMatrix();

    //position AR camera
    //ofMatrix4x4 -> float[16] seems to be broken :-(
    float *f = arMatrix.getPtr();
    glMultMatrixf(f); //ugh

    //reduce opacity
    ofSetColor(255, 255, 255, 100);
    ofPushMatrix();
		//ofTranslate(model.getPosition().x+100, model.getPosition().y, 0);
		//ofRotate(-mouseX, 0, 1, 0);
		//ofTranslate(-model.getPosition().x, -model.getPosition().y, 0);
    
		//model.drawFaces();
		model.drawWireframe();
            
    ofPopMatrix();
    ofPopMatrix();

    int ypos = 15;
    if (!drawCamera)
    	ofDrawBitmapString("Camera is off", 10, ypos); ypos +=15;

    ofDrawBitmapString("fps: "+ofToString(ofGetFrameRate(), 2), 10, ypos); ypos +=15;
    //ofDrawBitmapString("keys 1-5 load models, spacebar to trigger animation", 10, ypos); ypos +=15;
    //ofDrawBitmapString("drag to control animation with mouseY", 10, ypos); ypos +=15;
    ofDrawBitmapString("num animations for this model: " + ofToString(model.getAnimationCount()), 10, ypos); ypos +=15;

    //ofDrawBitmapString(ofToString(getIpAddress()), 10, ypos); ypos+=15;
    //ofDrawBitmapString("clients: " + ofToString(tcp.getNumClients()), 10, ypos); ypos+=15;

    //tcpConnected
    ofDrawBitmapString(ofToString(tcpConnected), 10, ypos); ypos+=15;

    //ofDrawBitmapString(ofToString(arMatrix), 10, ypos); ypos+=15;
    //char s[1000]; //it's 2011
    //sprintf(s, "%f %f %f %f, %f %f %f %f, %f %f %f %f, %f %f %f %f", f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8], f[9], f[10], f[11], f[12], f[13], f[14], f[15]);
    //ofDrawBitmapString(ofToString(s, 16), 10, ypos); ypos+=15;
    ofDrawBitmapString(debugMessage, 10, ypos); ypos+=15;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case '1':
            model.loadModel("astroBoy_walk.dae");
            model.setPosition(ofGetWidth()/2, (float)ofGetHeight() * 0.75 , 0);
            ofEnableSeparateSpecularLight();
            break;
        case '2':
            model.loadModel("TurbochiFromXSI.dae");
            model.setPosition(ofGetWidth()/2, (float)ofGetHeight() * 0.75 , 0);
            model.setRotation(0,90,1,0,0);
            ofEnableSeparateSpecularLight();
            break;
        case '3':
            model.loadModel("dwarf.x");
            model.setPosition(ofGetWidth()/2, (float)ofGetHeight() * 0.75 , 0);
            ofDisableSeparateSpecularLight();
            break;
        case '4':
            model.loadModel("monster-animated-character-X.X");
            model.setPosition(ofGetWidth()/2, (float)ofGetHeight() * 0.75 , 0);
            ofDisableSeparateSpecularLight();
            break;
		case '5':
			model.loadModel("squirrel/NewSquirrel.3ds");
		    model.setPosition(ofGetWidth()/2, (float)ofGetHeight() * 0.75 , 0);
            model.setRotation(0,-90,1,0,0);
            ofDisableSeparateSpecularLight();
			break;
		case ' ':
			bAnimate = !bAnimate;
			break;
        default:
            break;
    }


	mesh = model.getMesh(0);
	position = model.getPosition();
	normScale = model.getNormalizedScale();
	scale = model.getScale();
	sceneCenter = model.getSceneCenter();
	material = model.getMaterialForMesh(0);
    tex = model.getTextureForMesh(0);

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

char *testApp::getIpAddress()
{
	char ac[100];
	gethostname(ac, sizeof(ac));
	struct hostent* host = gethostbyname(ac);
	char** current_addr = host->h_addr_list;
	if (*current_addr != NULL)
	{
		struct in_addr* addr = (struct in_addr*)(*current_addr);
		return inet_ntoa(*addr);
	}
	else return NULL;
}

//msg should be 16 floats, space-delimited
ofMatrix4x4 testApp::extractMatrix(string msg)
{
	//debugMessage = msg;

	ofLog(OF_LOG_WARNING, "In extractMatrix");
	ofLog(OF_LOG_WARNING, msg);

	vector<float> matrix;

	//str2char*
	char *data = new char[msg.length()];
	int len = msg.copy(data, msg.length());
	data[len] = 0;

	//finally, parse!
	//break on space
	char *f_str = strtok(data, " ");
	while (f_str != NULL)
	{
		float f;
		//EOF means not found
		if (sscanf(f_str, "%f", &f) != EOF)
			matrix.push_back(f);

		f_str = strtok (NULL, " ");
	}

	//at this point matrix should be 4x4
	if (matrix.size() == 16)
	{
		ofLog(OF_LOG_WARNING, "Got Matrix");
		float tmp[16];
		memcpy( tmp, &matrix[0], sizeof(float) * 16 );

		return ofMatrix4x4(tmp);
	}
	else {
		ofLog(OF_LOG_WARNING, "did not Got Matrix");
		return ofMatrix4x4();
	}
}

void testApp::smoothARMatrix()
{
}
}

