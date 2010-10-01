#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxTSPSReceiver.h"
#include "ofxTriangle.h"
#include "ofxInteractiveSilhouette.h"
#include "ofxBox2d.h"

class CustomCircle : public ofxBox2dCircle{
	
public:
	ofPoint color;
	CustomCircle() {
		color = 0xFF6677;
	}
	void draw() {
		
		if(dead && body == NULL) return;
		
		float radius = getRadius();
		
		glPushMatrix();
		glTranslatef(getPosition().x, getPosition().y, 0);
		
		if(bIsFixed) {
			ofSetColor(255, 0, 255);
			ofFill();
			ofCircle(0, 0, radius);	
		}
		else {
			ofSetColor(color.x, color.y, color.z);
			ofNoFill();
			ofCircle(0, 0, radius);
			
			ofSetColor(color.x, color.y, color.z);
			ofFill();
			ofCircle(0, 0, radius/10.0);
			
			ofSetColor(color.x, color.y, color.z);
			ofNoFill();
			ofCircle(0, 0, radius/5.0);
		}
		
		glPopMatrix();
		
		float angle			= getRotation();
		const b2XForm& xf	= body->GetXForm();
		b2Vec2	center		= body->GetPosition();
		b2Vec2	axis		= xf.R.col1;
		b2Vec2	p			= center + radius/OFX_BOX2D_SCALE * axis;
		
		//ofSetColor(0xff00ff);
		//ofLine(getPosition().x, getPosition().y, p.x*OFX_BOX2D_SCALE, p.y*OFX_BOX2D_SCALE);
	}
};
class testApp : public ofBaseApp, public ofxTSPSListener
{

  public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void addCircle(int pointX, int pointY, ofPoint color);
	void processPerson( ofxTSPSPerson * person);
	ofxTSPSReceiver* receiver;
	
	ofxBox2d				 physics;
	vector <CustomCircle>  circles;
	vector <ofxBox2dPolygon * > polys;
	
	
	//called when the person enters the system
	void personEntered( ofxTSPSPerson* person, ofxTSPSScene* scene );
	//called each time the centroid moves (a lot)
	void personMoved( ofxTSPSPerson* person, ofxTSPSScene* scene );
	//called one frame before the person is removed from the list to let you clean up
	void personWillLeave( ofxTSPSPerson* person, ofxTSPSScene* scene );
	//called every frame no matter what.
	void personUpdated(ofxTSPSPerson* person, ofxTSPSScene* scene);
	
};

#endif
