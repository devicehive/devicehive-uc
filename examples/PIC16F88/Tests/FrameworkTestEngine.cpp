#include "stdafx.h"
#include <string.h>
//#include "Message.h"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;
using namespace System::IO::Ports;
using namespace System::ComponentModel;
using namespace System::Threading;

#include "ICommunication.h"
#include "FrameworkTestEngine.h"
#include <stdlib.h>
#include <memory.h>

namespace Tests
{
	FrameworkTestEngine::FrameworkTestEngine(ICommunication ^ com)
	{
		_com = com;
		_rd = NULL;
		_nd = NULL;
		_sendChecksum = 0;
		_recvChecksum = 0;
	}

	FrameworkTestEngine::~FrameworkTestEngine()
	{
		//_com->Close();
		FreeRegData();
		FreeNotificationData();
	}


	void FrameworkTestEngine::SendMessageHeader(WORD Intent, WORD Length)
	{
		MessageHeader Msg;
		Msg.Signature1 =  DEVICEHIVE_SIGNATURE1; //.Signature = DEVICEHIVE_SIGNATURE; //('E'<< 8) | 'C';
		Msg.Signature2 = DEVICEHIVE_SIGNATURE2;
		Msg.Version = DEVICEHIVE_VERSION;
		Msg.Intent = Intent;
		//memset(Msg.Reserved, 0, 6);
		//Msg.Reserved = res;
		Msg.Length = Length;
		//Msg.Check = CalcHeaderCheck(Msg);
		ResetChecksum();	
		//_com->SendBytes(&Msg, MESSAGE_HEADER_SIZE);
		SendBytes(&Msg, sizeof(MessageHeader));
		
		//Thread::Sleep(50);
	}

	MessageHeader FrameworkTestEngine::ReceiveMessageHeader()
	{
		int n = sizeof(MessageHeader);
		MessageHeader msgh;

		ResetChecksum();
		ReceiveBytes(&msgh, n);

		//_com->ReceiveBytes(&msgh, MESSAGE_HEADER_SIZE);
		return msgh;
	}

	RegData * FrameworkTestEngine::CreateRegData()
	{
		FreeRegData();

		_rd = new RegData;
		_rd->DeviceKey = NULL;
		_rd->DeviceName = NULL;
		_rd->DeviceClassName = NULL;
		_rd->DeviceClassVersion = NULL;
		_rd->Equipment.Length = 0;
		_rd->Equipment.Items = NULL;
		_rd->Notifications.Length = 0;
		_rd->Notifications.Items = NULL;
		_rd->Commands.Length = 0;
		_rd->Commands.Items = NULL;

		return _rd;
	}

	void FrameworkTestEngine::FreeEquipment(EquipmentArray & arr)
	{
		if (arr.Length > 0)
		{
			for (DWORD x = 0; x < arr.Length; ++x)
			{
				if (arr.Items[x].Name) delete [] arr.Items[x].Name;
				if (arr.Items[x].Code) delete [] arr.Items[x].Code;
				if (arr.Items[x].TypeName) delete [] arr.Items[x].TypeName;
			}
			delete [] arr.Items;
		}
	}

	void FrameworkTestEngine::FreeNotifications(NotificationArray & arr)
	{
		if (arr.Length > 0)
		{
			for (DWORD x = 0; x < arr.Length; ++x)
			{
				if (arr.Items[x].Name) delete [] arr.Items[x].Name;
				FreeParameters(arr.Items[x].Parameters);
			}
			delete [] arr.Items;
		}
	}

	void FrameworkTestEngine::FreeCommands(CommandArray & arr)
	{
		if (arr.Length > 0)
		{
			for (DWORD x = 0; x < arr.Length; ++x)
			{
				if (arr.Items[x].Name) delete [] arr.Items[x].Name;
				FreeParameters(arr.Items[x].Parameters);
			}
			delete [] arr.Items;
		}
	}

	void FrameworkTestEngine::FreeParameters(ParameterArray & arr)
	{
		if (arr.Length > 0)
		{
			for (DWORD x = 0; x < arr.Length; ++x)
			{
				if (arr.Items[x].Name) delete [] arr.Items[x].Name;
			}
			delete [] arr.Items;
		}
	}

	void FrameworkTestEngine::FreeRegData()
	{
		if (!_rd) return;
		if (_rd->DeviceKey) delete [] _rd->DeviceKey;
		if (_rd->DeviceName) delete [] _rd->DeviceName;
		if (_rd->DeviceClassName) delete [] _rd->DeviceClassName;
		if (_rd->DeviceClassVersion) delete [] _rd->DeviceClassVersion;

		FreeEquipment(_rd->Equipment);
		FreeNotifications(_rd->Notifications);
		FreeCommands(_rd->Commands);
		delete _rd;
		_rd = NULL;
	}

	RegData * FrameworkTestEngine::ReceiveRegData(DWORD size)
	{
		unsigned char * buf = NULL;
		try
		{
			buf = ReceiveMessageBody(size);
			ReceiveChecksum();
			//buf = new unsigned char[size];
			_rd = CreateRegData();
			//_com->ReceiveBytes(buf, size);

			void * v = (void*)buf;

			_rd->DeviceID = ParseStatic<GUID>(&v);
			_rd->DeviceKey = ParseString(&v);
			_rd->DeviceName = ParseString(&v);
			_rd->DeviceClassName = ParseString(&v);
			_rd->DeviceClassVersion = ParseString(&v);

			ParseArray<EquipmentArray, Equipment>(&_rd->Equipment, &v);
			for (WORD x = 0; x < _rd->Equipment.Length; ++x)
			{
				ParseEquipment(&_rd->Equipment.Items[x], &v);
			}

			ParseArray<NotificationArray, Notification>(&_rd->Notifications, &v);
			for (WORD x = 0; x < _rd->Notifications.Length; ++x)
			{
				ParseNotification(&_rd->Notifications.Items[x], &v);
			}

			ParseArray<CommandArray, Command>(&_rd->Commands, &v);
			for (WORD x = 0; x < _rd->Commands.Length; ++x)
			{
				ParseCommand(&_rd->Commands.Items[x], &v);
			}
			return _rd;
		}
		catch (...)
		{
			FreeRegData();
			throw;
		}
		finally
		{
			if (buf) delete [] buf;
		}
	}

	NotificationData * FrameworkTestEngine::CreateNotificationData()
	{
		_nd = new NotificationData;
		_nd->Status = NULL;
		_nd->Result = NULL;
		return _nd;
	}

	void FrameworkTestEngine::FreeNotificationData()
	{
		if (!_nd) return;
		if (_nd->Status) delete [] _nd->Status;
		if (_nd->Result) delete [] _nd->Result;
		delete _nd;
		_nd = NULL;

	}

	NotificationData * FrameworkTestEngine::ReceiveNotificationData(DWORD size)
	{
		unsigned char * buf = NULL;
		try
		{
			buf = ReceiveMessageBody(size);
			ReceiveChecksum();
			FreeNotificationData();
			_nd = CreateNotificationData();

			void * v = (void*) buf;
			_nd->CommandID = ParseStatic<DWORD>(&v);
			_nd->Status = ParseString(&v);
			_nd->Result = ParseString(&v);

			return _nd;
		}
		finally
		{
			if (buf) delete[] buf;
		}
	}

	// Memory parsing functions
	char * FrameworkTestEngine::ParseString(void ** v)
	{
		int n = sizeof(WORD);
		WORD len = ParseStatic<WORD>(v);

		char * rv = new char[len + 1];
		memcpy(rv, (char*)(*v), len);
		rv[len] = 0;
		*v = (char*)*v + len;
		return rv;
	}
	
	template <class T> T FrameworkTestEngine::ParseStatic(void **v)
	{
		T rv = *((T*)*v);
		*v = (char*)(*v) + sizeof(rv);
		return rv;
	}

	template <class U, class T> void FrameworkTestEngine::ParseArray(U * arr, void **v)
	{
		arr->Length = ParseStatic<WORD>(v);
		arr->Items = new T[arr->Length];
	}

	void FrameworkTestEngine::ParseEquipment(Equipment * eqp, void **v)
	{
		eqp->Name = ParseString(v);
		eqp->Code = ParseString(v);
		eqp->TypeName = ParseString(v);
	}
	
	void FrameworkTestEngine::ParseNotification(Notification * n, void **v)
	{
		n->Intent = ParseStatic<WORD>(v);
		n->Name = ParseString(v);
		ParseArray<ParameterArray, Parameter>(&n->Parameters, v);
		for (WORD x = 0; x < n->Parameters.Length; ++x)
		{
			ParseParameter(&n->Parameters.Items[x], v);
		}
	}

	void FrameworkTestEngine::ParseCommand(Command * c, void **v)
	{
		c->Intent = ParseStatic<WORD>(v);
		c->Name = ParseString(v);
		ParseArray<ParameterArray, Parameter>(&c->Parameters, v);
		for (DWORD x = 0; x < c->Parameters.Length; ++x)
		{
			ParseParameter(&c->Parameters.Items[x], v);
		}
	}

	void FrameworkTestEngine::ParseParameter(Parameter * p, void **v)
	{
		p->Type = ParseStatic<BYTE>(v);
		p->Name = ParseString(v);
	}

	unsigned char * FrameworkTestEngine::ReceiveMessageBody(DWORD length)
	{
		unsigned char * buf = new unsigned char[length];
		//_com->ReceiveBytes(buf, length);
		ReceiveBytes(buf, length);
		return buf;
	}
	
	void FrameworkTestEngine::WaitForData()
	{
		_com->WaitForData();
	}

	void FrameworkTestEngine::FlushData()
	{
		_com->Flush();
	}

	BYTE FrameworkTestEngine::CalcHeaderCheck(MessageHeader & h)
	{
		//BYTE *chk = (BYTE *)&h.Intent;
		//BYTE rv = chk[0];
		//for (BYTE x = 1; x < CHECK_HEADER_SIZE; ++x)
		//{
		//	rv ^= chk[x];
		//}
		//return rv;
		return true;
	}

	DWORD FrameworkTestEngine::GetStringLength(const char * s)
	{
		return sizeof(WORD) + strlen(s);
	}

	void FrameworkTestEngine::SendString(const char * s)
	{
		WORD len = strlen(s);
		//_com->SendBytes(&len, sizeof(DWORD));
		//_com->SendBytes((void*)s, len);
		SendBytes(&len, sizeof(WORD));
		SendBytes((void*)s, len);
	}

	BYTE FrameworkTestEngine::CalcDataChecksum(void * data, int length)
	{
		BYTE rv = 0;
		BYTE * bts = (BYTE*) data;
		for (int x = 0; x < length; ++x)
		{
			rv +=bts[x];
		}
		return rv;
	}

	void FrameworkTestEngine::SendBytes(void * data, int size)
	{
		_sendChecksum += CalcDataChecksum(data, size);
		_com->SendBytes(data, size);
	}

	bool FrameworkTestEngine::ReceiveBytes(void * data, int size)
	{
		
		bool rv = _com->ReceiveBytes(data, size);
		if (rv)
		{
			_recvChecksum += CalcDataChecksum(data, size);
		}
		return rv;
	}

	void FrameworkTestEngine::ResetChecksum(bool rx, bool tx)
	{
		if (rx) _recvChecksum = 0;
		if (tx) _sendChecksum = 0;
	}

	void FrameworkTestEngine::ResetChecksum()
	{
		ResetChecksum(true, true);
	}

	void FrameworkTestEngine::SendChecksum()
	{
		BYTE cs = ((BYTE) 0xFF) - _sendChecksum;
		SendBytes(&cs, sizeof(cs));
	}

	void FrameworkTestEngine::ReceiveChecksum()
	{
		BYTE rv = 0;
		ReceiveBytes(&rv, sizeof(BYTE));
		
		if (rv != 0)
		{
			Assert::AreEqual((BYTE)0xFF, _recvChecksum, "Checksum failed!");
		}
	}

}