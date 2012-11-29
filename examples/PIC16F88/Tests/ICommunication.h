#include "stdafx.h"

#ifndef __ICOMMUNICATION_H__
#define __ICOMMUNICATION_H__

//
//#include "Message.h"

//using namespace System;
//using namespace System::Text;
//using namespace System::Collections::Generic;
//using namespace Microsoft::VisualStudio::TestTools::UnitTesting;
//using namespace System::IO::Ports;
//using namespace System::ComponentModel;

namespace Tests
{
	public interface class ICommunication
	{
	public:
		bool Open();
		void Close();

		void SendByte(unsigned char ch);
		void SendBytes(void * buffer, int length);

		unsigned char ReceiveByte();
		bool ReceiveBytes(void * buffer, int length);

		void WaitForData();
		void Flush();
	};
}

#endif