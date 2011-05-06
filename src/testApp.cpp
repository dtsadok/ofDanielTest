#include "testApp.h"


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

	//tcpConnected = tcp.setup(HOST_IP, PORT);
	tcp.setup(11999);
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

	/*
	if (tcpConnected)
	{
		//call/response (?)
		string msg = tcp.receive();
		if( msg.length() > 0 )
		{
			arMatrix = extractMatrix(msg);
		}
	}
	else //try to reconnect
	{
		if  (ofGetElapsedTimeMillis() - connectTime > WAIT_TIME)
		{
			//ofLog("Trying to connect to host...");
			tcpConnected = tcp.setup(HOST_IP, PORT);
			connectTime = ofGetElapsedTimeMillis();
		}
	}
	*/

	if (tcp.getNumClients() > 0)
	{
		tcp.send(0, "hello client - you are connected on port - " + ofToString(tcp.getClientPort(0)) );
	}
}

//--------------------------------------------------------------
void testApp::draw(){
    //ofBackground(50, 50, 50, 0);
	ofBackground(0, 0, 0);
    ofSetColor(255, 255, 255, 255);
    
    grabber.draw((1024-800)/2, 0);

    ofPushMatrix();

    //position AR camera
    glMultMatrixf(arMatrix.getPtr()); //ugh

    //reduce opacity
    ofSetColor(255, 255, 255, 100);
    ofPushMatrix();
		ofTranslate(model.getPosition().x+100, model.getPosition().y, 0);
		ofRotate(-mouseX, 0, 1, 0);
		ofTranslate(-model.getPosition().x, -model.getPosition().y, 0);
    
		model.drawFaces();
            
    ofPopMatrix();
    ofPopMatrix();

    int ypos = 15;
    ofDrawBitmapString("fps: "+ofToString(ofGetFrameRate(), 2), 10, ypos); ypos +=15;
    //ofDrawBitmapString("keys 1-5 load models, spacebar to trigger animation", 10, ypos); ypos +=15;
    //ofDrawBitmapString("drag to control animation with mouseY", 10, ypos); ypos +=15;
    ofDrawBitmapString("num animations for this model: " + ofToString(model.getAnimationCount()), 10, ypos); ypos +=15;

    char ac[80];
    gethostname(ac, sizeof(ac));
    ofDrawBitmapString(ac, 10, ypos);

	if (tcp.getNumClients() > 0)
	{
		string port = ofToString( tcp.getClientPort(0) );
		string ip = tcp.getClientIP(0);
		string info = "client connected from "+ip+" on port: "+port;

		string msg = tcp.receive(0);
	}

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

//msg should be 16 floats, space-delimited
ofMatrix4x4 testApp::extractMatrix(string msg)
{
	vector<float> matrix;
	//break on space
	char *data = new char[msg.length()];
	int len = msg.copy(data, msg.length());
	data[len] = 0;

	//finally, parse!
	char *f_str = strtok(data, " ");
	while (f_str != NULL)
	{
		float f;
		//printf ("%s\n", f_str);
		//check for EOF from sccanf
		if (sscanf(f_str, "%f", &f) != EOF)
			matrix.push_back(f);

		f_str = strtok (NULL, " ");
	}

	//at this point matrix should be 4x4
	if (matrix.size() == 16)
	{
		float tmp[16];
		memcpy( tmp, &matrix[0], sizeof(float) * 16 );

		return ofMatrix4x4(tmp);
	}
}

