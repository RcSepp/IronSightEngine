#include <ISEngine.h>

void IObjectManager::RegisterObject(IManagedObject* obj)
{
	objectregister.push_back(obj);
}

void IObjectManager::UnregisterObject(IManagedObject* obj)
{
	objectregister.remove(obj);
}

void IObjectManager::ReleaseRegisteredObjects()
{
	/*while(objectregister.size())
		objectregister.front()->Release();*/
}

void IManagedObject::Unregister()
{
	//manager->UnregisterObject(this);
}