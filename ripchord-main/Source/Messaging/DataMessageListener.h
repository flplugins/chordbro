#pragma once

#include "JuceHeader.h"

class DataMessage;
class DataMessageManager;

//==============================================================================
class DataMessageListener : private AsyncUpdater
{
public:
    //==============================================================================
    DataMessageListener();
    virtual ~DataMessageListener();

    //==============================================================================
    virtual void handleNewMessage (const DataMessage* message) = 0;

private:
    //==============================================================================
    friend class DataMessageManager;

    void enqueueAsyncMessage (std::unique_ptr<DataMessage> message);
    void handleAsyncUpdate() override;

    CriticalSection mPendingMessagesLock;
    std::vector<std::unique_ptr<DataMessage>> mPendingMessages;

    //==============================================================================
    JUCE_DECLARE_WEAK_REFERENCEABLE (DataMessageListener)
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DataMessageListener);
};
