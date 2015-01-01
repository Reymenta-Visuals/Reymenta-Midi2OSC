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
	ss << "setupMidi" << std::endl;

	if (mMidiIn.getNumPorts() > 0)
	{
		mMidiIn.listPorts();
		for (int i = 0; i < mMidiIn.getNumPorts(); i++)
		{
			if (mParameterBag->mMIDIOpenAllInputPorts)
			{
				mMidiIn.openPort(i);
				mMidiIn.midiSignal.connect(boost::bind(&ReymentaMidi2OSCApp::midiListener, this, boost::arg<1>::arg()));
				ss << "Opening MIDI port " << i << " " << mMidiIn.mPortNames[i] << std::endl;
			}
			else
			{
				ss << "Available MIDI port " << i << " " << mMidiIn.mPortNames[i] << std::endl;
			}
		}
	}
	else
	{
		ss << "No MIDI Ports found!!!!" << std::endl;
	}
	mLogMsg = ss.str();
}
void ReymentaMidi2OSCApp::midiListener(midi::Message msg){
	float normalizedValue;
	stringstream ss;
	string controlType = "unknown";

	newLogMsg = true;
	ss << "midi port: " << msg.port << " ch: " << msg.channel << " status: " << msg.status;
	ss << " byteOne: " << msg.byteOne << " byteTwo: " << msg.byteTwo << std::endl;
	ss << " control: " << msg.control << " value: " << msg.value << std::endl;
	mLogMsg = ss.str();
	stringstream ssName;
	int name;
	int newValue;
	string controlName;
	BYTE b;

	switch (msg.status)
	{
	case MIDI_CONTROL_CHANGE:
		controlType = "cc";
		name = msg.control;
		newValue = msg.value;
		ssName << name;
		controlName = ssName.str();
		normalizedValue = lmap<float>(newValue, 0.0, 127.0, 0.0, 1.0);

		if (normalizedValue != controlValues[name])
		{
			controlValues[name] = newValue;
			//mOSC->sendOSCMidiMessage("/midi", name, newValue, msg.status, msg.channel);
			mOSC->sendOSCFloatMessage(controlType, name, normalizedValue, msg.channel);

		}
		break;
	case MIDI_NOTE_ON:
		controlType = "on";
		b = msg.byteOne;
		name = msg.pitch;
		newValue = msg.velocity;
		normalizedValue = lmap<float>(newValue, 0.0, 127.0, 0.0, 1.0);
		ssName << name;
		controlName = ssName.str();

		// send to OSC for resolume
		mOSC->sendOSCIntMessage(controlType, 1, 1, 1);

		break;
	case MIDI_NOTE_OFF:
		controlType = "off";
		b = msg.byteOne;


		break;
	default:
		break;

	}
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
		
		static ImVec4 color0 = ImVec4(0.352941, 0.160784, 0.160784, 1);
		static ImVec4 color1 = ImVec4(0.184314, 0.0392157, 0.0392157, 1);
		static ImVec4 color2 = ImVec4(0.239216, 0.0745098, 0.054902, 1);
		static ImVec4 color3 = ImVec4(0.69, 0.69, 0.69, 1);
		static ImVec4 color4 = ImVec4(0.407843, 0.270588, 0.270588, 1);

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

			// TODO add btn for mMidiIn.listPorts();
			for (int i = 0; i < mMidiIn.getNumPorts(); i++)
			{
				ImGui::Text(mMidiIn.mPortNames[i].c_str()); ImGui::NextColumn();
				char buf[32];
				sprintf_s(buf, "Connect %d", i);

				if (ImGui::Button(buf))
				{
					mMidiIn.openPort(i);
					mMidiIn.midiSignal.connect(boost::bind(&ReymentaMidi2OSCApp::midiListener, this, boost::arg<1>::arg()));
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
