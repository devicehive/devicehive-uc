//#include "stdafx.h"

#ifndef __COM_PORT_COMMUNICATION_H__
#define __COM_PORT_COMMUNICATION_H__
//#include "Message.h"
#include "ICommunication.h"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;
using namespace System::IO::Ports;
using namespace System::ComponentModel;
using namespace System::Threading;

namespace Tests
{
	public ref class ComPortCommunication : ICommunication, IDisposable
	{
	private:
		SerialPort ^ _com;
		AutoResetEvent ^ _evt;

		void OnReceiveData(Object ^ sender, SerialDataReceivedEventArgs ^ e);
	public:
		ComPortCommunication();
		~ComPortCommunication();

		virtual bool Open();

		virtual void Close();

		virtual void SendByte(unsigned char ch);

		virtual void SendBytes(void * buffer, int length);

		virtual unsigned char ReceiveByte();

		virtual bool ReceiveBytes(void * buffer, int length);

		virtual void WaitForData();

		virtual void Flush();
	};
}

#endif