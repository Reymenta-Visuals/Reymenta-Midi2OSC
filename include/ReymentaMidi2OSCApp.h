#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "OSCSender.h"
#include "MidiIn.h"
#include "MidiMessage.h"
#include "MidiConstants.h"

#include <list>

// UserInterface
#include "imGuiCinder.h"
// parameters
#include "ParameterBag.h"
// OSC
#include "OSCWrapper.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace Reymenta;

class ReymentaMidi2OSCApp : public AppNative {
public:
	void prepareSettings(Settings* settings);
	void setup();
	void update();
	void draw();
	void shutDown();
	void keyDown(KeyEvent event);
	void processMidiMessage(midi::Message* message);
	void quitProgram();
	void sendOSCMessage(string controlType, int controlName, int controlValue0, int controlValue1 = 0);
	void sendOSCMidiMessage(string controlType, int controlName, int controlValue0, int controlValue1 = 0, int channel = 0);

	map<int, int> controlValues;

	//std::vector<std::string>					destinationHosts;
	//std::vector<int>							destinationPorts;
	// midi
	midi::Input mMidiIn;
	void setupMidi();
	void midiListener(midi::Message msg);


private:
	int							nanoPort;
	string						nanoPortName;
	std::vector< osc::Sender>	mOSCSenders;

	// Mouse
	bool						mMouseDown;
	Vec2i						mMousePos;
	// update timer
	double						lastUpdate;
	int							mUpdateInterval;

	string						mLogMsg;
	bool						newLogMsg;
};