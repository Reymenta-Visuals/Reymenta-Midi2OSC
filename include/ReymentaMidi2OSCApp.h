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

	map<int, float> controlValues;

	// midi
	midi::Input mMidiIn;
	void setupMidi();
	void midiListener(midi::Message msg);

private:
	// parameters
	ParameterBagRef				mParameterBag;
	// osc
	OSCRef						mOSC;

	string						mLogMsg;
	bool						newLogMsg;
};