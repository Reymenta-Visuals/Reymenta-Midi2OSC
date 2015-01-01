#include "ReymentaMidi2OSCApp.h"

void ReymentaMidi2OSCApp::prepareSettings(Settings* settings){
	settings->setFrameRate(1200.0f);
	settings->setWindowSize(880, 510);
	settings->setWindowPos(Vec2i(0, 30));
}

void ReymentaMidi2OSCApp::setup()
{
	
	getWindow()->setTitle("Reymenta midi2osc");
	newLogMsg = false;

	mUpdateInterval = 2.0f;
	lastUpdate = getElapsedSeconds();
	mMousePos = Vec2i::zero();
	mMouseDown = false;

	// set ui window and io events callbacks
	ImGui::setWindow(getWindow());
	// OSC sender
	destinationHosts.push_back("127.0.0.1");
	destinationPorts.push_back(7000);
	mOSCSenders.push_back(osc::Sender());
	mOSCSenders[0].setup(destinationHosts[0], destinationPorts[0]);
	destinationHosts.push_back("127.0.0.1");
	destinationPorts.push_back(9000);
	mOSCSenders.push_back(osc::Sender());
	mOSCSenders[1].setup(destinationHosts[1], destinationPorts[1]);

	nanoPort = 0;
	//mOSCSender.setup(destinationHost, destinationPort);

	// midi
	setupMidi();

	vector<float> mbuffer;
	for (int i = 0; i < 1024; i++)
	{
		mbuffer.push_back(0);
	}

}
void ReymentaMidi2OSCApp::setupMidi()
{
	// midi
	nanoPort = 0;
	if (mMidiIn.getNumPorts() > 0)
	{
		mMidiIn.listPorts();
		for (int i = 0; i < mMidiIn.getNumPorts(); i++)
		{
			console() << "port:" << mMidiIn.mPortNames[i] << std::endl;
			size_t found = mMidiIn.mPortNames[i].find("nano");
			if (found != string::npos)
			{
				nanoPort = i;
				nanoPortName = mMidiIn.mPortNames[i];
			}
		}

		//mMidiIn.openPort(nanoPort);
		//mMidiIn.midiSignal.connect(boost::bind(&ReymentaMIDIApp::midiListener, this, boost::arg<1>::arg()));

		//console() << "Opening MIDI port " << nanoPort << " " << nanoPortName << std::endl;
	}
	else
	{
		//console() << "No MIDI Ports found!!!!" << std::endl;
	}
}
void ReymentaMidi2OSCApp::midiListener(midi::Message msg){
	newLogMsg = true;
	/*switch (msg.status)
	{
	case MIDI_NOTE_ON:
	notes[msg.pitch] = msg.velocity;
	status = "Pitch: " + toString(msg.pitch) + "\n" +
	"Velocity: " + toString(msg.velocity);
	break;
	case MIDI_NOTE_OFF:
	break;
	case MIDI_CONTROL_CHANGE:
	cc[msg.control] = msg.value;
	status = "Control: " + toString(msg.control) + "\n" +
	"Value: " + toString(msg.value);
	break;
	default:
	break;
	}*/
	stringstream ss;
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
		name = msg.control;
		newValue = msg.value;
		ssName << name;
		controlName = ssName.str();
		if (newValue != controlValues[name])
		{
			controlValues[name] = newValue;
			sendOSCMidiMessage("/midi", name, newValue, msg.status, msg.channel);

		}

		// slider
		if (name < 9)
		{
			if (newValue != controlValues[name])
			{
				controlValues[name] = newValue;
				sendOSCMessage("/slider", name, newValue);
				float colorValue = newValue / 127.0;
				switch (name)
				{
				case 1:
					iBackgroundColor.r = colorValue;
					break;
				case 2:
					iBackgroundColor.g = colorValue;
					break;
				case 3:
					iBackgroundColor.b = colorValue;
					break;
				case 4:
					iColor.r = colorValue;
					break;
				case 5:
					iColor.g = colorValue;
					break;
				case 6:
					iColor.b = colorValue;
					break;
				default:
					break;

				}
			}
		}
		// toggle
		if (name > 20 && name < 49)
		{
			if (newValue != controlValues[name])
			{
				controlValues[name] = newValue;
				toggle->setValue(newValue);
				sendOSCMessage("/toggle", name, newValue);
			}
		}
		// rotary
		if (name > 10 && name < 19)
		{
			if (newValue != controlValues[name])
			{
				controlValues[name] = newValue;
				rotary->setValue(newValue);
				sendOSCMessage("/rotary", name, newValue);
			}
		}
		break;
	case MIDI_NOTE_ON:
		b = msg.byteOne;

		//keybd_event( mJson->keyboardValues[ b ], NULL, NULL, NULL );

		break;
	case MIDI_NOTE_OFF:
		b = msg.byteOne;


		break;
	default:
		break;

	}
}
void ReymentaMidi2OSCApp::sendOSCMidiMessage(string controlType, int controlName, int controlValue0, int controlValue1, int channel)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addIntArg(controlName);
	m.addIntArg(controlValue0);
	m.addIntArg(controlValue1);
	m.addIntArg(channel);
	mOSCSenders[0].sendMessage(m);
	mOSCSenders[1].sendMessage(m);
}
void ReymentaMidi2OSCApp::sendOSCMessage(string controlType, int controlName, int controlValue0, int controlValue1)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addIntArg(controlName);
	m.addIntArg(controlValue0);
	mOSCSenders[0].sendMessage(m);
	mOSCSenders[1].sendMessage(m);
	status->setLabel(controlType);
}
void ReymentaMidi2OSCApp::update()
{
	getWindow()->setTitle("(" + toString(floor(getAverageFps())) + " fps) Reymenta midi2osc");
}

void ReymentaMidi2OSCApp::draw(){
	gl::clear(ColorAf(0.0f, 0.0f, 0.0f, 0.0f));

	gl::setViewport(getWindowBounds());
	gl::setMatricesWindow(getWindowSize());
	// ci UI
	//gui->draw();
	//imgui
	static float f = 0.0f;

	ImGui::NewFrame();

	// start a new window
	ImGui::Begin("Spout parameters", NULL, ImVec2(200, 100));
	{
		// our theme variables
		/*
		static ImVec4 color0 = ImVec4(0.352941, 0.160784, 0.160784, 1);
		static ImVec4 color1 = ImVec4(0.184314, 0.0392157, 0.0392157, 1);
		static ImVec4 color2 = ImVec4(0.239216, 0.0745098, 0.054902, 1);
		static ImVec4 color3 = ImVec4(0.69, 0.69, 0.69, 1);
		static ImVec4 color4 = ImVec4(0.407843, 0.270588, 0.270588, 1);*/

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

			mMidiIn.listPorts();
			for (int i = 0; i < mMidiIn.getNumPorts(); i++)
			{
				ImGui::Text(mMidiIn.mPortNames[i].c_str()); ImGui::NextColumn();
				char buf[32];
				sprintf_s(buf, "Connect %d", i);

				if (ImGui::Button(buf))
				{
					mMidiIn.openPort(i);
					mMidiIn.midiSignal.connect(boost::bind(&ReymentaMIDIApp::midiListener, this, boost::arg<1>::arg()));
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
	ImGui::Shutdown();

}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( ReymentaMidi2OSCApp, RendererGl )
