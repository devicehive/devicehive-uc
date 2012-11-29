#include "stdafx.h"
#include "Message.h"
#include "ICommunication.h"
#include "ComPortCommunication.h"
#include <memory.h>
#include <string.h>
#include "DeviceConstants.h"
#include "DeviceSpecificTestEngine.h"
#include "BtnTestForm.h"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;
using namespace System::IO::Ports;
using namespace System::ComponentModel;
using namespace System::Threading;
//using namespace System::Configuration;

namespace Tests
{
	[TestClass]
	public ref class SimpleDeviceTest
	{
	private:
		TestContext^ testContextInstance;
		ICommunication ^ com;
		//DeviceSpecificTestEngine ^ fte;

	public: 
		/// <summary>
		///Gets or sets the test context which provides
		///information about and functionality for the current test run.
		///</summary>
		property Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ TestContext
		{
			Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ get()
			{
				return testContextInstance;
			}
			System::Void set(Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ value)
			{
				testContextInstance = value;
			}
		};

		#pragma region Additional test attributes
		//
		//You can use the following additional attributes as you write your tests:
		//
		//Use ClassInitialize to run code before running the first test in the class
		//[ClassInitialize()]
		//static void MyClassInitialize(TestContext^ testContext) {};
		//
		//Use ClassCleanup to run code after all tests in a class have run
		//[ClassCleanup()]
		//static void MyClassCleanup() {};
		//
		//Use TestInitialize to run code before running each test
		[TestInitialize()]
		void MyTestInitialize() 
		{
			com = gcnew ComPortCommunication;
			//fte = gcnew FrameworkTestEngine(com);
			com->Open();
		}
		//
		//Use TestCleanup to run code after each test has run
		[TestCleanup()]
		void MyTestCleanup() 
		{
			com->Close();
		};
		
		#pragma endregion 

		[TestMethod]
		void Register()
		{
			//DWORD ddw = crc_table(1);
			DeviceSpecificTestEngine ^ fte;
			try
			{
				
				fte = gcnew DeviceSpecificTestEngine(com);
				fte->FlushData();
				//{
				fte->SendMessageHeader(G2D_REQREG, 0);
				fte->SendChecksum(); // valid message is: C5 C3 01 00 00 00 00 00 76
				Thread::Sleep(100);
				MessageHeader msgh = fte->ReceiveMessageHeader();

				
				//Assert::IsTrue( msgh.Length < 2048, "Message is too large to be received!" );
				CheckHeader(msgh, fte);
				Assert::IsTrue(msgh.Intent == D2G_REGISTER, "Register intent expected");
				Assert::IsTrue( msgh.Length > 0, "Response data length should not be zero!" );

				Thread::Sleep(200);

				RegData * rd = fte->ReceiveRegData(msgh.Length);
				CheckRegistrationData(rd);
				
				
				MessageHeader nh1 = fte->ReceiveMessageHeader();
				CheckHeader(nh1, fte);
				EquipmentNotification * en1 = fte->ReceiveEquipmentNotification(nh1.Length);

				//delete en1;

				MessageHeader nh2 = fte->ReceiveMessageHeader();
				CheckHeader(nh2, fte);
				EquipmentNotification * en2 = fte->ReceiveEquipmentNotification(nh2.Length);
				//delete en2
					//fte->Dispose();
				//}
			}
			finally
			{
				fte->~DeviceSpecificTestEngine();
			}
		};

		NotificationData * ReceiveNotification(FrameworkTestEngine ^ fte)
		{
			MessageHeader msgh = fte->ReceiveMessageHeader();
			CheckHeader(msgh, fte);
			Thread::Sleep(200);
			Assert::IsTrue(msgh.Intent == D2G_NOTIFY, "Invalid intent. Notification expected!");
			Assert::IsTrue(msgh.Length > 0, "Response data length should not be zero!");

			NotificationData * nd = fte->ReceiveNotificationData(msgh.Length);
			return nd;
		}

		/*[TestMethod]
		void TestUnknownMessage()
		{
			DeviceSpecificTestEngine ^ fte;
			try
			{
				const DWORD fakeCommand = 10;
				fte = gcnew DeviceSpecificTestEngine(com);
				fte->SendMessageHeader(fakeCommand, 0);
				Thread::Sleep(50);
				
				NotificationData * nd = ReceiveNotification(fte);
				Assert::IsTrue(nd->CommandID == 0, "Invalid command ID");
				Assert::IsTrue(strcmp(nd->Result, ERR_FAIL) == 0, "Invalid response - fail expected!");

			}
			finally
			{
				fte->~DeviceSpecificTestEngine();
			}
		}*/

		EquipmentNotification * ReceiveEquipmentNotification(DeviceSpecificTestEngine ^ fte)
		{
			MessageHeader msgh = fte->ReceiveMessageHeader();
			CheckHeader(msgh, fte);
			Assert::IsTrue(msgh.Length > 0, "Response data length should not be zero!");
			Assert::AreEqual(msgh.Intent, (WORD)D2G_EQUIPMENT, "Invalid intent. Equipment notification expected!");
			EquipmentNotification * rv = fte->ReceiveEquipmentNotification(msgh.Length);

			return rv;
		}

		void TestLed(DeviceSpecificTestEngine ^ fte, DWORD id, BOOL state)
		{
			//fte->SendMessageHeader(G2D_LED, 1);
			//Thread::Sleep(50);
			fte->SendLedCommand(id, LED_EQP_CODE, state);
			//fte->WaitForData();
			Thread::Sleep(200);

			NotificationData * nd1 = ReceiveNotification(fte);
			Assert::IsTrue(nd1->CommandID == id, "Invalid command ID returned.");
			Assert::IsTrue(strcmp(nd1->Result, RESULT_OK) == 0, "Invalid result. OK expected.");
			
			//fte->WaitForData();
			Thread::Sleep(200);
			EquipmentNotification * en = ReceiveEquipmentNotification(fte);

			Assert::IsTrue(strcmp(en->EquipmentCode, LED_EQP_CODE) == 0, "Invalid equipment code. LED code expected!");
			Assert::IsTrue(en->State == state, "Invalid LED state!");

		}

		[TestMethod]
		void TestLED()
		{
			DeviceSpecificTestEngine ^ fte;
			try
			{
				fte = gcnew DeviceSpecificTestEngine(com);
				TestLed(fte, 1, TRUE);
				Thread::Sleep(500);
				TestLed(fte, 2, FALSE);
			}
			finally
			{
				fte->~DeviceSpecificTestEngine();
			}
		}

		[TestMethod]
		void TestButton()
		{
			DeviceSpecificTestEngine ^ fte;
			try
			{
				//TestContext->WriteLine("Press a button on device...");
				BtnTestForm ^ frm = gcnew BtnTestForm;
				frm->Show();
				fte = gcnew DeviceSpecificTestEngine(com);
				fte->FlushData();
				fte->WaitForData();
				Thread::Sleep(200);

				EquipmentNotification * en1 = ReceiveEquipmentNotification(fte);
				frm->Close();
				Assert::IsTrue(strcmp(en1->EquipmentCode, BTN_EQP_CODE) == 0, "Press - invalid equipment code. Button code expected!");
				Assert::IsTrue(en1->State == TRUE, "Invalid Button state! True expected.");
				//
				
				fte->WaitForData();
				Thread::Sleep(200);
				EquipmentNotification * en2 = ReceiveEquipmentNotification(fte);

				Assert::IsTrue(strcmp(en2->EquipmentCode, BTN_EQP_CODE) == 0, "Release - invalid equipment code. Button code expected!");
				Assert::IsTrue(en2->State == FALSE, "Invalid Button state! False expected.");
			}
			finally
			{
				fte->~DeviceSpecificTestEngine();
			}
			//fte->WaitForData();

		}

		void CheckHeader(MessageHeader &h, FrameworkTestEngine ^ fte)
		{
			//Assert::IsTrue( h.Signature == DEVICEHIVE_SIGNATURE, String::Format("Invalid signature - {0}. {1} expected", h.Signature, DEVICEHIVE_SIGNATURE));
			
			Assert::IsTrue( h.Signature1 == MessageFormat::DEVICEHIVE_SIGNATURE1, String::Format("Invalid signature first byte - {0}.", h.Signature1));
			Assert::IsTrue( h.Signature2 == MessageFormat::DEVICEHIVE_SIGNATURE2, String::Format("Invalid signature second byte - {0}.", h.Signature2));
			Assert::IsTrue( h.Version == MessageFormat::DEVICEHIVE_VERSION, String::Format("Invalid version - {0}.", h.Version));
			//Assert::IsTrue( h.Check == fte->CalcHeaderCheck(h), "Invalid header checksum!");
		}

		void CheckRegistrationData(RegData * rd)
		{
			Assert::IsTrue(memcmp(&rd->DeviceID, &DeviceID, sizeof(GUID)) == 0, "Invalid GUID!");
			Assert::IsTrue(strcmp(rd->DeviceKey, DEVICE_KEY) == 0, "Invalid device key!");
			Assert::IsTrue(strcmp(rd->DeviceName, DEVICE_NAME) == 0, "Invalid device name!");
			
			Assert::IsTrue(rd->Equipment.Length == EQUIPMENT_LENGTH, "Invalid equipment count!");
						
			Assert::IsTrue(rd->Notifications.Length == NOTIFICATIONS_COUNT, "Invalid notifications count!");
			Assert::IsTrue(rd->Notifications.Items[0].Parameters.Length == NPEQ_COUNT, "Invalid number of parameters of the notification!");
			
			Assert::IsTrue(rd->Commands.Length == COMMANDS_COUNT, "Invalid commands count!");
		}
	};
}
