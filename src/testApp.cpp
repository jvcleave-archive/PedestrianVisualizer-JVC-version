#include "testApp.h"
ofxBox2dLine lineStrip;
vector<ofPoint> circlePoints;
//--------------------------------------------------------------
void testApp::setup(){
	
	//basic drawing stuff
	ofEnableSmoothing();
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();

	//set up the box2d environment
	physics.init();
	physics.createBounds();
	physics.setGravity(0, 10);
	physics.checkBounds(true);
	
	
	//set up the receiver to listen for people
	receiver = new ofxTSPSReceiver();
	receiver->setListener( this );
	receiver->connect(12000);
	
}

//--------------------------------------------------------------
void testApp::update(){
	
	for (int i = polys.size()-1; i >= 0; i--) {
		physics.world->DestroyBody(polys[i]->body);
	}
	
	polys.clear();
	
	for(int i = 0; i < receiver->totalPeople(); i++){

		//get the person and their cv contour
		ofxTSPSPerson* person = receiver->personAtIndex(i);
		ofxInteractiveSilhouette* silhouette = (ofxInteractiveSilhouette*)person->customAttributes;
		vector<ofPoint> contourToTriangulate = person->denormalizedContour(ofGetWidth(), ofGetHeight());

		if(i % 2 == 0)
		{
			//addCircle(person->boundingRect.x *ofGetWidth(), person->centroid.y*ofGetHeight());
			//cout << "person->centroid.x" << person->centroid.x << "person->centroid.y" << person->centroid.y <<endl;
		}
		
		//if we have a valid poly...
		if(contourToTriangulate.size() > 2){
			//clear the existing triangle set for that person and create a new one
			silhouette->triangle.clear();
			silhouette->triangle.triangulate( contourToTriangulate, 50 );
			silhouette->hasTriangle = true;
			
			//for all the triangles add them to the physics world
			for(int t = silhouette->triangle.triangles.size()-1; t >= 0; t--) {
				ofxTriangleData& tData = silhouette->triangle.triangles[t];
				
				ofxBox2dPolygon * poly = new ofxBox2dPolygon();
				poly->addVertex(tData.a.x, tData.a.y);
				poly->addVertex(tData.b.x, tData.b.y);
				poly->addVertex(tData.c.x, tData.c.y);
				person->hasPoly = true;
				person->poly = poly;
				if(poly->validateShape()) {
					poly->createShape(physics.world, 0.0f, 0.0f);
					polys.push_back(poly);
				}
			}
		}
	}

	physics.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(255, 255, 255);

	for(int i = 0; i < receiver->totalPeople(); i++){
		ofxTSPSPerson* person = receiver->personAtIndex(i);
		ofxInteractiveSilhouette* silhouette = (ofxInteractiveSilhouette*)person->customAttributes;
		if(silhouette->hasTriangle){
			//silhouette->triangle.draw();
		}
		
		ofSetColor(silhouette->color.x, silhouette->color.y, silhouette->color.z);
		for(int c = 1; c < person->contour.size(); c++)
		{
			ofLine(person->contour[c-1].x*ofGetWidth(), person->contour[c-1].y*ofGetHeight(), 
				   person->contour[c  ].x*ofGetWidth(), person->contour[c  ].y*ofGetHeight());
		}
	}
	
	for(int i = 0; i < circles.size(); i++){
		circles[i].draw();
	}

	/*lineStrip.clear();
	for(int i=0; i<segs; i++) {
		p.x += 15;
		lineStrip.addPoint(p.x, p.y+sin(i*ofRandom(0.01, 0.5))*30);
	}*/
	
	/*if (circlePoints.size() > 0) {
		
		lineStrip.setWorld(physics.getWorld());
		lineStrip.clear();
		lineStrip.bIsLoop = true;
		ofPoint p(circlePoints[0].x, circlePoints[0].y);
		for(int i = 0; i < circlePoints.size()-1; i++)
		{
			p.x = circlePoints[i].x;
			if (i % 2 == 0) {
				lineStrip.addPoint(p.x, circlePoints[i].y);
			}
			//ofLine(circlePoints[i].x, circlePoints[i].y, circlePoints[i+1].x, circlePoints[i+1].y);
			//ofLine(ofGetWidth()/2, ofGetHeight()/2, circlePoints[i+1].x, circlePoints[i+1].y);
		}
		
		lineStrip.createShape();

	}*/
}

//called when the person enters the system
void testApp::personEntered( ofxTSPSPerson* person, ofxTSPSScene* scene )
{
	//put a color into the custom attributes field
	ofxInteractiveSilhouette* silhouette = new ofxInteractiveSilhouette();
	silhouette->color = ofPoint(ofRandom(0, 255),ofRandom(0, 255),ofRandom(0, 255));
	person->customAttributes = silhouette;
	
}

//called each time the centroid moves (a lot)
void testApp::personMoved( ofxTSPSPerson* person, ofxTSPSScene* scene )
{
	//cout << "personMoved" << "person.contour.size" << person->contour.size() << endl;
	processPerson(person);
	
	
}

void testApp::processPerson( ofxTSPSPerson * person)
{
	if(person->hasPoly)
	{
		cout << "HAS POLY" <<endl;
		//person->poly->addAttractionPoint(person->centroid.x, 100);
		for (int i = 0; i<circles.size(); i++) {
			
			if (i % 2 ==0) {
				circles[i].addAttractionPoint(ofPoint(person->centroid.x*ofGetWidth(), person->centroid.y*ofGetHeight())
											  , 5);
			}
			
			
		}

	}else {
		cout << "NO POLY" <<endl;
	}

	if (person->hasDrawn == false) {
		vector<ofPoint> contourToTriangulate = person->denormalizedContour(ofGetWidth(), ofGetHeight());
		person->hasDrawn = true;
		//lineStrip.destroyShape();
		int numPoints = contourToTriangulate.size();
		for (int i = 0; i<numPoints; i++) {
			//if(i % 2  == 0)
			//{
			if(circles.size()<400)
			{
				circlePoints.push_back(contourToTriangulate[i]);
				ofxInteractiveSilhouette* silhouette = (ofxInteractiveSilhouette*)person->customAttributes;
				addCircle(contourToTriangulate[i].x, contourToTriangulate[i].y, silhouette->color);	
			}else {
				for (int j = 0; j<circles.size(); j++) {
					circles[j].destroyShape();
				}
				circles.clear();
				circlePoints.clear();
			}
			
			//}
		}
	}
}

//called one frame before the person is removed from the list to let you clean up
void testApp::personWillLeave( ofxTSPSPerson* person, ofxTSPSScene* scene )
{
	delete (ofxInteractiveSilhouette*)person->customAttributes;
}

//called every frame no matter what.
void testApp::personUpdated(ofxTSPSPerson* person, ofxTSPSScene* scene)
{
	//processPerson(person);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if(key == 'c')
	{
		//addCircle(mouseX, mouseY, ofPoint(0, 0, 0);
	}
	
}

void testApp::addCircle(int pointX, int pointY, ofPoint color)
{
	CustomCircle circle;
	circle.color = color;
	circle.setPhysics(3.0, 0.53, 0.1);
	circle.setup(physics.getWorld(), pointX, pointY, ofRandom(5, 5), false);
	circles.push_back(circle);
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

