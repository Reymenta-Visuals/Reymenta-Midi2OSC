#include "ReymentaMidi2OSCApp.h"

void ReymentaMidi2OSCApp::prepareSettings(Settings* settings){
	settings->setFrameRate(1200.0f);
	// parameters
	mParameterBag = ParameterBag::create();

	settings->setWindowSize(880, 510);
	settings->setWindowPos(Vec2i(0, 30));
}

void ReymentaMidi2OSCApp::setup()
{

	getWindow()->setTitle("Reymenta midi2osc");
	getWindow()->connectClose(&ReymentaMidi2OSCApp::shutdown, this);
	// instanciate the OSC class
	mOSC = OSC::create(mParameterBag);

	newLogMsg = false;

	// set ui window and io events callbacks
	ImGui::setWindow(getWindow());

	// midi
	setupMidi();
}
void ReymentaMidi2OSCApp::setupMidi()
{
	stringstream ss;
	newLogMsg = true;
	ss << "setupMidi: ";

	if (mMidiIn0.getNumPorts() > 0)
	{
		mMidiIn0.listPorts();
		for (int i = 0; i <mMidiIn0.getNumPorts(); i++)
		{

			midiInput mIn;
			mIn.portName = mMidiIn0.mPortNames[i];
			mMidiInputs.push_back(mIn);
			if (mParameterBag->mMIDIOpenAllInputPorts)
			{
				if (i == 1)
				{
					mMidiIn1.openPort(i);
					mMidiIn1.midiSignal.connect(boost::bind(&ReymentaMidi2OSCApp::midiListener, this, boost::arg<1>::arg()));
				}
				if (i == 2)				{
					mMidiIn2.openPort(i);
					mMidiIn2.midiSignal.connect(boost::bind(&ReymentaMidi2OSCApp::midiListener, this, boost::arg<1>::arg()));
				}
				mMidiInputs[i].isConnected = true;
				ss << "Opening MIDI port " << i << " " << mMidiInputs[i].portName ;
			}
			else
			{
				mMidiInputs[i].isConnected = false;
				ss << "Available MIDI port " << i << " " << mMidiIn0.mPortNames[i] ;
			}
		}
	}
	else
	{
		ss << "No MIDI Ports found!!!!" << std::endl;
	}
	ss << std::endl;
	mLogMsg = ss.str();
}
void ReymentaMidi2OSCApp::midiListener(midi::Message msg)
{
	float normalizedValue;
	stringstream ss;
	ss << "midi port: " << msg.port << " ch: " << msg.channel << " status: " << msg.status;
	string controlType = "unknown";

	newLogMsg = true;
	int name;
	int newValue;

	switch (msg.status)
	{
	case MIDI_CONTROL_CHANGE:
		controlType = "cc";
		name = msg.control;
		newValue = msg.value;
		break;
	case MIDI_NOTE_ON:
		controlType = "on";
		name = msg.pitch;
		newValue = msg.velocity;
		break;
	case MIDI_NOTE_OFF:
		controlType = "off";
		name = msg.pitch;
		newValue = msg.velocity;
		break;
	default:
		break;
	}
	normalizedValue = lmap<float>(newValue, 0.0, 127.0, 0.0, 1.0);
	ss << " control: " << name << " value: " << newValue << " normalized: " << normalizedValue << std::endl;
	mLogMsg = ss.str();
	mOSC->sendOSCFloatMessage(controlType, name, normalizedValue, msg.channel);
}

void ReymentaMidi2OSCApp::update()
{
	getWindow()->setTitle("(" + toString(floor(getAverageFps())) + " fps) Reymenta midi2osc");
}

void ReymentaMidi2OSCApp::draw(){
	gl::clear(ColorAf(0.0f, 0.0f, 0.0f, 0.0f));

	gl::setViewport(getWindowBounds());
	gl::setMatricesWindow(getWindowSize());

	//imgui
	static float f = 0.0f;

	ImGui::NewFrame();

	// start a new window
	ImGui::Begin("MIDI2OSC", NULL, ImVec2(getWindowWidth(), getWindowHeight()));
	{
		// our theme variables
		static float WindowPadding[2] = { 25, 10 };
		static float WindowMinSize[2] = { 160, 80 };
		static float FramePadding[2] = { 4, 4 };
		static float ItemSpacing[2] = { 10, 5 };
		static float ItemInnerSpacing[2] = { 5, 5 };

		static float WindowFillAlphaDefault = 0.7;
		static float WindowRounding = 4;
		static float TreeNodeSpacing = 22;
		static float ColumnsMinSpacing = 50;
		static float ScrollBarWidth = 12;

		if (ImGui::CollapsingHeader("MidiIn", "0", true, true))
		{
			ImGui::Columns(2, "data", true);
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("Connect"); ImGui::NextColumn();
			ImGui::Separator();

			for (int i = 0; i < mMidiInputs.size(); i++)
			{
				ImGui::Text(mMidiInputs[i].portName.c_str()); ImGui::NextColumn();
				char buf[32];
				if (mMidiInputs[i].isConnected)
				{
					sprintf_s(buf, "Disconnect %d", i);
				}
				else
				{
					sprintf_s(buf, "Connect %d", i);
				}

				if (ImGui::Button(buf))
				{
					stringstream ss;
					if (mMidiInputs[i].isConnected)
					{
						if (i == 0)
						{
							mMidiIn0.closePort();
						}
						if (i == 1)
						{
							mMidiIn1.closePort();
						}
						if (i == 2)
						{
							mMidiIn2.closePort();
						}
						mMidiInputs[i].isConnected = false;
					}
					else
					{
						if (i == 0)
						{
							mMidiIn0.openPort(i);
							mMidiIn0.midiSignal.connect(boost::bind(&ReymentaMidi2OSCApp::midiListener, this, boost::arg<1>::arg()));
						}
						if (i == 1)
						{
							mMidiIn1.openPort(i);
							mMidiIn1.midiSignal.connect(boost::bind(&ReymentaMidi2OSCApp::midiListener, this, boost::arg<1>::arg()));
						}
						if (i == 2)
						{
							mMidiIn2.openPort(i);
							mMidiIn2.midiSignal.connect(boost::bind(&ReymentaMidi2OSCApp::midiListener, this, boost::arg<1>::arg()));
						}
						mMidiInputs[i].isConnected = true;
						ss << "Opening MIDI port " << i << " " << mMidiInputs[i].portName << std::endl;
					}
					mLogMsg = ss.str();
				}
				ImGui::NextColumn();
				ImGui::Separator();
			}
			ImGui::Columns(1);

		}
		if (ImGui::CollapsingHeader("Log", "1", true, true))
		{
			static ImGuiTextBuffer log;
			static int lines = 0;
			ImGui::Text("Buffer contents: %d lines, %d bytes", lines, log.size());
			if (ImGui::Button("Clear")) { log.clear(); lines = 0; }
			//ImGui::SameLine();

			if (newLogMsg)
			{
				newLogMsg = false;
				log.append(mLogMsg.c_str());
				lines++;
				if (lines > 20) { log.clear(); lines = 0; }
			}
			ImGui::BeginChild("Log");
			ImGui::TextUnformatted(log.begin(), log.end());
			ImGui::EndChild();
		}
	}
	ImGui::End();
	ImGui::Render();
}

void ReymentaMidi2OSCApp::keyDown(KeyEvent event)
{
}

void ReymentaMidi2OSCApp::shutDown()
{
	mParameterBag->save();
	ImGui::Shutdown();
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC(ReymentaMidi2OSCApp, RendererGl)
