#ifndef __DEVICE_SPECIFIC_TEST_ENGINE_H__
#define __DEVICE_SPECIFIC_TEST_ENGINE_H__

//#include "datatypes.h"

#include "FrameworkTestEngine.h"

namespace Tests
{

	public ref class DeviceSpecificTestEngine : FrameworkTestEngine
	{
	private:
		EquipmentNotification * _en;
		void FreeEquipmentNotification();
		EquipmentNotification * CreateEquipmentNotification();

	public:
		DeviceSpecificTestEngine(ICommunication ^ com);
		~DeviceSpecificTestEngine();

		void SendLedCommand(DWORD id, const char * equipment, BOOL state);
		EquipmentNotification * ReceiveEquipmentNotification(DWORD size);
	};
}

#endif