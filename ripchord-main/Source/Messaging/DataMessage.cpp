#include "DataMessage.h"

//==============================================================================
DataMessage::DataMessage()
{
}

DataMessage::~DataMessage()
{
}

std::unique_ptr<DataMessage> DataMessage::createCopy() const
{
    auto copy = std::make_unique<DataMessage>();
    copy->broadcaster = broadcaster;
    copy->messageCode = messageCode;
    copy->listenerType = listenerType;
    copy->messageVar1 = messageVar1;
    copy->messageVar2 = messageVar2;
    copy->messageVar3 = messageVar3;
    copy->messageVar4 = messageVar4;
    copy->messageArray1 = messageArray1;
    copy->messageArray2 = messageArray2;
    return copy;
}
