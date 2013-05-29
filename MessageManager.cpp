#include "MessageManager.h"

using namespace ECS;

void MessageManager::connect()
{
  /*
	for (auto iter = mFunctions.begin(); iter != mFunctions.end(); ++iter)
	{
		(*iter)();
	}
	*/

	for (auto iter = mReceivers.begin(); iter != mReceivers.end(); ++iter)
	{
		(*iter)->connect(mMessages);
	}
	mMessages.clear();

}