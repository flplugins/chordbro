#include "DataMessageListener.h"
#include "DataMessageManager.h"
#include "DataMessage.h"

//==============================================================================
DataMessageListener::DataMessageListener()
{
}

DataMessageListener::~DataMessageListener()
{
    if (auto* messageManager = DataMessageManager::getInstanceWithoutCreating())
    {
        messageManager->removeListener (this);
    }

    cancelPendingUpdate();

    const CriticalSection::ScopedLockType lock (mPendingMessagesLock);
    mPendingMessages.clear();
}

void DataMessageListener::enqueueAsyncMessage (std::unique_ptr<DataMessage> message)
{
    {
        const CriticalSection::ScopedLockType lock (mPendingMessagesLock);
        mPendingMessages.push_back (std::move (message));
    }

    triggerAsyncUpdate();
}

void DataMessageListener::handleAsyncUpdate()
{
    std::vector<std::unique_ptr<DataMessage>> pendingMessages;

    {
        const CriticalSection::ScopedLockType lock (mPendingMessagesLock);
        pendingMessages.swap (mPendingMessages);
    }

    for (const auto& message : pendingMessages)
    {
        handleNewMessage (message.get());
    }
}
