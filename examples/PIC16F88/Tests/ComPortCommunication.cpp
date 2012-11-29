#include "stdafx.h"
#include "ComPortCommunication.h"
////#include "Message.h"
//#include "ICommunication.h"
//
//using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;
using namespace System::IO::Ports;
using namespace System::ComponentModel;

namespace Tests
{
	ComPortCommunication::ComPortCommunication() 
	{
		
		Container^ cc = gcnew Container;
		_com = gcnew SerialPort(cc);
		_evt = gcnew AutoResetEvent(false);
		_com->PortName = Configuration::ConfigurationManager::AppSettings["ComPortName"];
		_com->BaudRate = Int32::Parse(Configuration::ConfigurationManager::AppSettings["ComPortBaud"]);
		_com->Parity = (Parity)Enum::Parse(Parity::typeid, Configuration::ConfigurationManager::AppSettings["ComPortParity"]);
		_com->DataBits = Int32::Parse(Configuration::ConfigurationManager::AppSettings["ComPortBits"]);
		_com->StopBits = (StopBits)Enum::Parse(StopBits::typeid, Configuration::ConfigurationManager::AppSettings["ComPortStopBits"]);
		_com->ReadTimeout = Int32::Parse(Configuration::ConfigurationManager::AppSettings["ReadTimeout"]);
		_com->WriteTimeout = Int32::Parse(Configuration::ConfigurationManager::AppSettings["WriteTimeout"]);
		_com->DataReceived::add(gcnew SerialDataReceivedEventHandler(this, &ComPortCommunication::OnReceiveData));
	}

	ComPortCommunication::~ComPortCommunication()
	{
		Close();
	}

	bool ComPortCommunication::Open()
	{
		_com->Open();
		_com->DiscardInBuffer();
		_com->DiscardOutBuffer();
		return true;
	}

	void ComPortCommunication::Close()
	{
		_com->DiscardInBuffer();
		_com->DiscardOutBuffer();
		_com->Close();
	}

	void ComPortCommunication::SendByte(unsigned char ch)
	{
		array<unsigned char>^ bts = gcnew array<unsigned char>(1);
		bts[0] = ch;
		_com->Write(bts, 0, 1);
	}

	void ComPortCommunication::SendBytes(void * buffer, int length)
	{
		array<unsigned char>^ bts = gcnew array<unsigned char>(length);
		for (int x = 0; x < length; ++x)
		{
			unsigned char ch = ((unsigned char*) (buffer))[x];
			bts[x] = ch;
		}
		_com->Write(bts, 0, length);
	}

	unsigned char ComPortCommunication::ReceiveByte()
	{
		unsigned char ch = _com->ReadByte();
		return ch;
	}

	bool ComPortCommunication::ReceiveBytes(void * buffer, int length)
	{
		array<unsigned char> ^ bts = gcnew array<unsigned char>(length);

		int rv = _com->Read(bts, 0, length);

		for (int x = 0; x < length; ++x)
		{
			((unsigned char*)buffer)[x] = bts[x];
		}
		return rv == length;
	}

	void ComPortCommunication::OnReceiveData(Object ^ sender, SerialDataReceivedEventArgs ^ e)
	{
		_evt->Set();
	}

	void ComPortCommunication::WaitForData()
	{
		_evt->WaitOne();
	}

	void ComPortCommunication::Flush()
	{
		_com->DiscardInBuffer();
		_com->ReadExisting();
	}
}

//
//namespace Tests
//{
//	public ref class ComPortCommunication : ICommunication
//	{
//	private:
//		SerialPort ^ _com;
//	public:
//		ComPortCommunication()
//		{
//			Container^ cc = gcnew Container;
//			_com = gcnew SerialPort(cc);
//
//			_com->PortName = Configuration::ConfigurationManager::AppSettings["ComPortName"];
//			_com->BaudRate = Int32::Parse(Configuration::ConfigurationManager::AppSettings["ComPortBaud"]);
//			_com->Parity = (Parity)Enum::Parse(Parity::typeid, Configuration::ConfigurationManager::AppSettings["ComPortParity"]);
//			_com->DataBits = Int32::Parse(Configuration::ConfigurationManager::AppSettings["ComPortBits"]);
//			_com->StopBits = (StopBits)Enum::Parse(StopBits::typeid, Configuration::ConfigurationManager::AppSettings["ComPortStopBits"]);
//			_com->ReadTimeout = Int32::Parse(Configuration::ConfigurationManager::AppSettings["ReadTimeout"]);
//			_com->WriteTimeout = Int32::Parse(Configuration::ConfigurationManager::AppSettings["WriteTimeout"]);
//
//		};
//
//		virtual bool Open()
//		{
//			_com->Open();
//			return true;
//		};
//
//		virtual void Close()
//		{
//			_com->Close();
//		};
//
//		virtual void SendByte(unsigned char ch)
//		{
//			array<unsigned char>^ bts = gcnew array<unsigned char>(1);
//			bts[0] = ch;
//			_com->Write(bts, 0, 1);
//		};
//
//		virtual void SendBytes(void * buffer, int offset, int length)
//		{
//			array<unsigned char>^ bts = gcnew array<unsigned char>(length);
//			for (int x = 0; x < length; ++x)
//			{
//				unsigned char ch = ((unsigned char*) (buffer))[offset + x];
//				bts[x] = ch;
//			}
//			_com->Write(bts, 0, 1);
//		};
//
//		virtual unsigned char ReceiveByte()
//		{
//			unsigned char ch = _com->ReadByte();
//			return ch;
//		};
//
//		virtual bool ReceiveBytes(void * buffer, int offset, int length)
//		{
//			array<unsigned char> ^ bts = gcnew array<unsigned char>(length);
//
//			int rv = _com->Read(bts, offset, length);
//
//			for (int x = 0; x < length; ++x)
//			{
//				((unsigned char*)buffer)[offset + x] = bts[x];
//			}
//			return rv == length;
//		};
//	};
//}
//
