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
#ifndef __NET_SERVICE_H__
#define __NET_SERVICE_H__

#include "connector/omicronConnectorClient.h"
#include "omicron/osystem.h"
#include "omicron/ServiceManager.h"
#include "omicron/InputServer.h"

namespace omicron
{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	class NetService: public Service, omicronConnector::IOmicronConnectorClientListener
	{
	public:
		// Allocator function
		NetService();
		static NetService* New() { return new NetService(); }

	public:
		virtual void onEvent(const omicronConnector::EventData& ed);
		virtual void setup(Setting& settings);
		virtual void initialize();
		virtual void poll();
		virtual void dispose();
		void setServer(const String& str, int );
		void setDataport(int);

	private:
		static NetService* mysInstance;

		omicronConnector::OmicronConnectorClient* myClient;

		String serverAddress;
		int serverPort;
		int dataPort;
		bool connected;
		bool dataStreamOut;
		int reconnectDelay;

		NetClient* streamClient;
	};

};

#endif
