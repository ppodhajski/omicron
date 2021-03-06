/**************************************************************************************************
 * THE OMICRON PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2017		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Arthur Nishimoto		anishimoto42@gmail.com
 *  Alessandro Febretti		febret@gmail.com
 *-------------------------------------------------------------------------------------------------
 * Copyright (c) 2010-2017, Electronic Visualization Laboratory, University of Illinois at Chicago
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this list of conditions 
 * and the following disclaimer. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the documentation and/or other 
 * materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF 
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************************************/
#include "omicron/WiimoteService.h"
#include "omicron/StringUtils.h"

using namespace omicron;


///////////////////////////////////////////////////////////////////////////////////////////////////
WiimoteService* WiimoteService::mysInstance = NULL;
int WiimoteService::maxControllers = 4;
std::map<int, wiimote> wiimoteInfo;

///////////////////////////////////////////////////////////////////////////////////////////////////
void wii_state_change (wiimote			  &remote,
					  state_change_flags  changed,
					  const wiimote_state &new_state)
{
	// we use this callback to set report types etc. to respond to key events
	//  (like the wiimote connecting or extensions (dis)connecting).
	
	// the wiimote just connected
	if(changed & WIIMOTE_CONNECTED)
		{
		// ask the wiimote to report everything (using the 'non-continous updates'
		//  default mode - updates will be frequent anyway due to the acceleration/IR
		//  values changing):

		// note1: you don't need to set a report type for Balance Boards - the
		//		   library does it automatically.
		
		// note2: for wiimotes, the report mode that includes the extension data
		//		   unfortunately only reports the 'BASIC' IR info (ie. no dot sizes),
		//		   so let's choose the best mode based on the extension status:
		if(new_state.ExtensionType != wiimote::BALANCE_BOARD)
			{
			if(new_state.bExtension)
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT); // no IR dots
			else
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);		//    IR dots
			}
		}
	// a MotionPlus was detected
	if(changed & MOTIONPLUS_DETECTED)
		{
		// enable it if there isn't a normal extension plugged into it
		// (MotionPlus devices don't report like normal extensions until
		//  enabled - and then, other extensions attached to it will no longer be
		//  reported (so disable the M+ when you want to access them again).
		if(remote.ExtensionType == wiimote_state::NONE) {
			bool res = remote.EnableMotionPlus();
			_ASSERT(res);
			}
		}
	// an extension is connected to the MotionPlus
	else if(changed & MOTIONPLUS_EXTENSION_CONNECTED)
		{
		// We can't read it if the MotionPlus is currently enabled, so disable it:
		if(remote.MotionPlusEnabled())
			remote.DisableMotionPlus();
		}
	// an extension disconnected from the MotionPlus
	else if(changed & MOTIONPLUS_EXTENSION_DISCONNECTED)
		{
		// enable the MotionPlus data again:
		if(remote.MotionPlusConnected())
			remote.EnableMotionPlus();
		}
	// another extension was just connected:
	else if(changed & EXTENSION_CONNECTED)
		{
		// switch to a report mode that includes the extension data (we will
		//  loose the IR dot sizes)
		// note: there is no need to set report types for a Balance Board.
		if(!remote.IsBalanceBoard())
			remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);
		}
	// extension was just disconnected:
	else if(changed & EXTENSION_DISCONNECTED)
		{
		// use a non-extension report mode (this gives us back the IR dot sizes)
		remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);
		}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WiimoteService::setup(Setting& settings)
{
	myUpdateInterval = Config::getFloatValue("updateInterval", settings, 0.1f);
	myEventSourceId = Config::getIntValue("eventSourceId", settings, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WiimoteService::initialize()
{
	omsg("WiimoteService::initialize()");
	mysInstance = this;

	/*
	for (int i = 0; i < maxControllers; i++)
	{
		wiimoteInfo[i] = wiimote();

		// in this demo we use a state-change callback to get notified of
		//  extension-related events, and polling for everything else
		// (note you don't have to use both, use whatever suits your app):
		wiimoteInfo[i].ChangedCallback = wii_state_change;
		//  notify us only when the wiimote connected sucessfully, or something
		//   related to extensions changes
		wiimoteInfo[i].CallbackTriggerFlags = (state_change_flags)(WIIMOTE_CONNECTED |
			EXTENSION_CHANGED |
			MOTIONPLUS_CHANGED);
		
		if (wiimoteInfo[i].Connect(i + 1))
		{
			ofmsg("WiimoteService: Wiimote %1% connected", %(i+1));

			switch (i+1)
			{
			case(1) :
				wiimoteInfo[i].SetLEDs(0x01);
				myWiimote = wiimoteInfo[i];
				myButtonState = pollButtonState();
				break;
			case(2) :
				wiimoteInfo[i].SetLEDs(0x02);
				break;
			case(3) :
				wiimoteInfo[i].SetLEDs(0x04);
				break;
			case(4) :
				wiimoteInfo[i].SetLEDs(0x08);
				break;
			default:
				break;
			}
			
		}
		else
		{
			ofmsg("WiimoteService: Wiimote %1% not connected", % (i + 1));
		}
		
	}
	*/
	
	/*
	if (myWiimote.Connect(1))
	{
		owarn("WiimoteService: Wiimote 1x connected");
		myWiimote.SetLEDs(0x01);
	}
	if (myWiimote.Connect(2))
	{
		owarn("WiimoteService: Wiimote 2x connected");
		myWiimote.SetLEDs(0x02);
	}
	if (myWiimote.Connect(3))
	{
		owarn("WiimoteService: Wiimote 3x connected");
	}
	if (myWiimote.Connect(4))
	{
		owarn("WiimoteService: Wiimote 4x connected");
	}
	*/

	// in this demo we use a state-change callback to get notified of
	//  extension-related events, and polling for everything else
	// (note you don't have to use both, use whatever suits your app):
	myWiimote.ChangedCallback = wii_state_change;
	//  notify us only when the wiimote connected sucessfully, or something
	//   related to extensions changes
	myWiimote.CallbackTriggerFlags = (state_change_flags)(WIIMOTE_CONNECTED |
		EXTENSION_CHANGED |
		MOTIONPLUS_CHANGED);

	if(!myWiimote.Connect(wiimote::FIRST_AVAILABLE)) 
	{
		owarn("WiimoteService: no wiimote controller connected.");
	}
	else
	{
		// Initialize button state
		myButtonState = pollButtonState();

		// Set all LEDs
		myWiimote.SetLEDs(0x01);
	}
	
	myUpdateTimer.start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint WiimoteService::pollButtonState()
{
	// Set controller type and button flags.
	uint flags = 0;
	if (myWiimote.Button.A()) flags |= Event::Button6;
	if (myWiimote.Button.B()) flags |= Event::Button7;
	if (myWiimote.Button.Plus()) flags |= Event::Button2;
	if (myWiimote.Button.Minus()) flags |= Event::Button3;
	if (myWiimote.Button.One()) flags |= Event::Button1;
	if (myWiimote.Button.Two()) flags |= Event::Button4;
	if (myWiimote.Button.Up()) flags |= Event::ButtonUp;
	if (myWiimote.Button.Down()) flags |= Event::ButtonDown;
	if (myWiimote.Button.Left()) flags |= Event::ButtonLeft;
	if (myWiimote.Button.Right()) flags |= Event::ButtonRight;

	if (myWiimote.NunchukConnected())
	{
		if (myWiimote.Nunchuk.C) flags |= Event::Button5;
		if (myWiimote.Nunchuk.Z) flags |= Event::Button7;
	}
	// 00 = off
	// 01 = LED 1
	// 02 = LED 2
	// 03 = LED 1, 2
	// 04 = LED 3
	// 05 = LED 1, 3
	// 06 = LED 2, 3
	// 07 = LED 1, 2, 3
	// 08 = LED 4
	// 09 = LED 1, 4
	// 0a = LED 2, 3
	// 0b = LED 1, 2, 4
	// 0c = LED 3, 4
	// 0d = LED 1, 3, 4
	// 0e = LED 2, 3, 4
	// 0f = LED 1, 2, 3, 4
	//myWiimote.SetLEDs(0x0f);
	return flags;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WiimoteService::poll() 
{
	if(myUpdateTimer.getElapsedTime() < myUpdateInterval) return;
	// Reset update timer.
	myUpdateTimer.stop();
	myUpdateTimer.start();

	if(myWiimote.IsConnected())
	{
		// Update the Wiimote state (this is essential for polling data)
		myWiimote.RefreshState();

		lockEvents();

		writeWiimoteEvent();
		//if(myWiimote.NunchukConnected())
		//{
			//writeNunchuckEvent();
		//}
		//if( myWiimote.MotionPlusConnected() )
		//{
			//writeMotionPlusEvent();
		//}

		unlockEvents();

		if(myWiimote.Button.Home())
		{
			myWiimote.SetLEDs(0);
			myWiimote.Disconnect();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WiimoteService::writeWiimoteEvent()
{
	Event* evt = writeHead();
	evt->reset(Event::Update, Service::Controller, myEventSourceId);

	// See if button state has changed and send Up / Down events accordingly. 
	// NOTE: this currently works correctly only if ONE button state changes during each
	// poll cycle.
	uint curButtonState = pollButtonState();
	if(curButtonState != myButtonState)
	{
		// If button state is bigger than previous state, it means one additional bit has been
		// set - so send a down event.
		if(curButtonState > myButtonState)
		{
			evt->reset(Event::Down, Service::Controller, myEventSourceId);
			if (isDebugEnabled()) ofmsg("Wiimote %1% flags %2% down event", %myEventSourceId %curButtonState);
		}
		else
		{
			evt->reset(Event::Up, Service::Controller, myEventSourceId);
			if (isDebugEnabled()) ofmsg("Wiimote %1% flags %2% up event", %myEventSourceId %curButtonState);
		}
		myButtonState = curButtonState;
	}
	else
	{
		// Button state has not changed, just send an update event.
		evt->reset(Event::Update, Service::Controller, myEventSourceId);
	}

	// Save digital information
	evt->setFlags(myButtonState);

	// Save analog information
	evt->setExtraDataType(Event::ExtraDataFloatArray);
	evt->setExtraDataFloat(0, myWiimote.Acceleration.X);
	evt->setExtraDataFloat(1, myWiimote.Acceleration.Y);
	evt->setExtraDataFloat(2, myWiimote.Acceleration.Z);
	evt->setExtraDataFloat(0, myWiimote.BatteryPercent);

	if (isDebugEnabled())
	{
		//ofmsg("Wiimote Accelorometer: %1% %2% %3%", %myWiimote.Acceleration.X %myWiimote.Acceleration.Y %myWiimote.Acceleration.Z );
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WiimoteService::writeNunchuckEvent()
{
	Event* evt = writeHead();
	evt->reset(Event::Update, Service::Controller, myEventSourceId);
	uint flags = TypeNunchuk;
	if(myWiimote.Nunchuk.C) flags |= ButtonA;
	if(myWiimote.Nunchuk.Z) flags |= ButtonB;
	evt->setFlags(flags);

	evt->setPosition(
		myWiimote.Nunchuk.Joystick.X,
		myWiimote.Nunchuk.Joystick.Y);

	evt->setExtraDataType(Event::ExtraDataFloatArray);
	evt->setExtraDataFloat(0, myWiimote.Nunchuk.Acceleration.X);
	evt->setExtraDataFloat(1, myWiimote.Nunchuk.Acceleration.Y);
	evt->setExtraDataFloat(2, myWiimote.Nunchuk.Acceleration.Z);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WiimoteService::writeMotionPlusEvent()
{
	Event* evt = writeHead();
	evt->reset(Event::Update, Service::Controller, myEventSourceId);
	uint flags = TypeMotionPlus;
	evt->setFlags(flags);

	evt->setPosition(
		myWiimote.MotionPlus.Raw.Yaw,
		myWiimote.MotionPlus.Raw.Pitch,
		myWiimote.MotionPlus.Raw.Roll);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WiimoteService::dispose() 
{
	mysInstance = NULL;
}