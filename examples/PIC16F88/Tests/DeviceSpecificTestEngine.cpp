#include "stdafx.h"
//#include "Message.h"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;
using namespace System::IO::Ports;
using namespace System::ComponentModel;
using namespace System::Threading;

//#include "ICommunication.h"
//#include "datatypes.h"
#include "DeviceConstants.h"
#include "FrameworkTestEngine.h"
#include "DeviceSpecificTestEngine.h"

#include <stdlib.h>
//#include <memory.h>

namespace Tests
{
	DeviceSpecificTestEngine::DeviceSpecificTestEngine(ICommunication ^ com) : FrameworkTestEngine(com)
	{
		_en = NULL;
	}

	DeviceSpecificTestEngine::~DeviceSpecificTestEngine()
	{
		FreeEquipmentNotification();
		//FrameworkTestEngine::~FrameworkTestEngine();
		
	}

	void DeviceSpecificTestEngine::SendLedCommand(DWORD id, const char * equipment, BOOL state)
	{
		SendMessageHeader(G2D_LED, sizeof(BYTE) + sizeof(DWORD) + GetStringLength(equipment));
		//Thread::Sleep(100);
		//_com->SendBytes(&id, sizeof (DWORD));
		SendBytes(&id, sizeof (DWORD));
		SendString(equipment);
		//_com->SendByte(state);
		SendBytes(&state, sizeof (BOOL));

		// Valid message should be: ?
		// C5 C3 01 01 01 0B 00 01 00 00 00 04 00 4C 45 44 31 01 5D
		SendChecksum();
	}

	EquipmentNotification * DeviceSpecificTestEngine::CreateEquipmentNotification()
	{
		FreeEquipmentNotification();
		_en = new EquipmentNotification;
		_en->EquipmentCode = NULL;
		return _en;
	}

	EquipmentNotification * DeviceSpecificTestEngine::ReceiveEquipmentNotification(DWORD size)
	{
		unsigned char * buf = NULL;
		try
		{
			buf = ReceiveMessageBody(size);
			ReceiveChecksum();
			void * v = (void *)buf;

			CreateEquipmentNotification();
			_en->EquipmentCode = ParseString(&v);
			_en->State = ParseStatic<BOOL>(&v);

			return _en;
		}
		finally
		{
			if (buf) delete [] buf;
		}
	}

	void DeviceSpecificTestEngine::FreeEquipmentNotification()
	{
		if (_en)
		{
			if (_en->EquipmentCode) delete [] _en->EquipmentCode;
			delete _en;
			_en = NULL;

		}
	}

}