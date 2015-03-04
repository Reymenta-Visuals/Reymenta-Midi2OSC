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
// WebSockets
#include "WebSocketsWrapper.h"
// Utils
#include "Batchass.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace Reymenta;

#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))

struct midiInput
{
	string			portName;
	bool			isConnected;
};
class ReymentaMidi2OSCApp : public AppNative {
public:
	void prepareSettings(Settings* settings);
	void setup();
	void update();
	void draw();
	void shutDown();
	void keyDown(KeyEvent event);

private:
	// parameters
	ParameterBagRef				mParameterBag;
	// osc
	OSCRef						mOSC;
	// WebSockets
	WebSocketsRef				mWebSockets;
	// utils
	BatchassRef					mBatchass;
	// midi
	vector<midiInput>			mMidiInputs;
	void						setupMidi();
	void						midiListener(midi::Message msg);
	// midi inputs: couldn't make a vector
	midi::Input					mMidiIn0;
	midi::Input					mMidiIn1;
	midi::Input					mMidiIn2;
	midi::Input					mMidiIn3;
	// log
	string						mLogMsg;
	bool						newLogMsg;

};