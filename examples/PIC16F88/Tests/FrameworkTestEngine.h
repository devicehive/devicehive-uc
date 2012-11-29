#ifndef __FRAMEWORK_TEST_ENGINE_H__
#define __FRAMEWORK_TEST_ENGINE_H__

#include "Message.h"
#include "ICommunication.h"

namespace Tests
{

	public ref class FrameworkTestEngine : IDisposable
	{
	private:
		
		RegData * _rd;
		NotificationData * _nd;
		void FreeRegData();
		void FreeNotificationData();
		RegData * CreateRegData();
		NotificationData * CreateNotificationData();

		BYTE _sendChecksum;
		BYTE _recvChecksum;

	protected:
		ICommunication ^ _com;
		// Cleanup functions
		
		void FreeEquipment(EquipmentArray & arr);
		void FreeNotifications(NotificationArray & arr);
		void FreeCommands(CommandArray & arr);
		void FreeParameters(ParameterArray & arr);
		
		// Memory parsing finctions
		char * ParseString(void ** v);
		template <class T> T ParseStatic(void **v);
		template <class U, class T> void ParseArray(U * arr, void **v);
		void ParseEquipment(Equipment * eqp, void **v);
		void ParseNotification(Notification * n, void **v);
		void ParseCommand(Command * c, void **v);
		void ParseParameter(Parameter * p, void **v);

		// Data transmission functions
		unsigned char * ReceiveMessageBody(DWORD length);
		//void SendByte(BYTE b);
		BYTE CalcDataChecksum(void * data, int length);
		

	public:
		FrameworkTestEngine(ICommunication ^ com);
		~FrameworkTestEngine();

		void SendBytes(void * data, int size);
		bool ReceiveBytes(void * data, int size);

		void SendMessageHeader(WORD Intent, WORD Length);
		MessageHeader ReceiveMessageHeader();
		
		DWORD GetStringLength(const char * s);
		void SendString(const char * s);
		
		RegData * ReceiveRegData(DWORD size);
		NotificationData * ReceiveNotificationData(DWORD size);

		BYTE CalcHeaderCheck(MessageHeader & h);

		void WaitForData();
		void FlushData();

		void ResetChecksum(bool rx, bool tx);
		void ResetChecksum();
		void SendChecksum();
		void ReceiveChecksum();

		//IDispose
		//virtual void Dispose();

	};
}

#endif