#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(25);
	ofBackground(0);
	ofSetWindowTitle("Insta");

	this->box2d.init();
	this->box2d.setGravity(0, 50);
	this->box2d.createBounds();
	this->box2d.setFPS(25);
	//this->box2d.registerGrabbing();

	this->cap.open("city3.mp4");
	this->cap_size = ofVec2f(this->cap.get(CV_CAP_PROP_FRAME_WIDTH), this->cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	this->cap_size *= 0.33;

	this->image.allocate(this->cap_size.x, this->cap_size.y, OF_IMAGE_COLOR);
	this->frame = cv::Mat(this->image.getHeight(), this->image.getWidth(), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);
	
	this->fbo.allocate(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::update() {

	cv::Mat src;
	this->cap >> src;
	
	if (src.empty()) {
		return;
	}

	cv::flip(src, src, 1);
	cv::cvtColor(src, src, cv::COLOR_BGR2RGB);
	cv::resize(src, this->frame, cv::Size(this->cap_size.x, this->cap_size.y));

	float span = 10;
	if(ofGetFrameNum() > 10) {

		cv::Mat gap = this->pre_frame - this->frame;
		vector<ofVec2f> gap_points;

		for (int y = 0; y < gap.rows; y += span) {

			cv::Vec3b* value = gap.ptr<cv::Vec3b>(y, 0);
			for (int x = 0; x < gap.cols; x += span) {

				cv::Vec3b v = value[x];
				if ((v[0] + v[1] + v[2]) > 32) {
					 
					shared_ptr<ofxBox2dCircle> circle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
					circle.get()->setPhysics(3.0, 0.5, 0.1);
					circle.get()->setup(this->box2d.getWorld(), x + ofGetWidth() / 2 - this->image.getWidth() / 2 + ofRandom(-span / 2, span / 2), y + 30 + ofRandom(-span / 2, span / 2), span / 4);
					this->circles.push_back(circle);

					this->circles_life.push_back(255);

					ofColor c;
					c.setHsb(ofRandom(255), 220, 220);
					this->circles_color.push_back(c);

				}
			}
		}
	}
	this->frame.copyTo(this->pre_frame);
	this->image.update();

	this->fbo.begin();
	ofClear(0);
	ofBackground(0);

	ofSetColor(255);
	this->image.draw(ofGetWidth() / 2 - this->image.getWidth() / 2, 30);

	for (int i = this->circles.size() - 1; i >= 0; i--) {
		
		this->circles_life[i] -= 8;
		if (this->circles_life[i] < 0) {

			this->circles[i].get()->destroy();
			this->circles.erase(this->circles.begin() + i);
			this->circles_life.erase(this->circles_life.begin() + i);
			this->circles_color.erase(this->circles_color.begin() + i);
		}
		else {

			ofSetColor(this->circles_color[i], this->circles_life[i] + 128);
			ofDrawCircle(this->circles[i].get()->getPosition(), this->circles[i].get()->getRadius());
		}
	}

	this->fbo.end();

	//this->fbo.readToPixels(pixels);
	//this->save_image.setFromPixels(pixels);
	//this->save_image.save(std::to_string(ofGetFrameNum()) + ".png", OF_IMAGE_QUALITY_BEST);

	this->box2d.update();
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->fbo.draw(0, 0);
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}