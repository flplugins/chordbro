#include "EditControlsComponent.h"

//==============================================================================
EditControlsComponent::EditControlsComponent (MainProcess& inMainProcess)
:   mMainProcess (inMainProcess),
    mGlobalState (mMainProcess.getGlobalState()),
    mPresetState (mMainProcess.getPresetState()),
    mMidiState (mMainProcess.getMidiState())
{
    mGlobalState.DataMessageBroadcaster::addListener (this, ListenerType::kSync);
    mPresetState.DataMessageBroadcaster::addListener (this, ListenerType::kSync);

    mImages.setDrawableButtonImages (mAllWhiteButton, "AllWhite.svg", "", "AllWhiteON.svg", "");
    mImages.setDrawableButtonImages (mAllBlackButton, "AllBlack.svg", "", "AllBlackON.svg", "");
    mImages.setDrawableButtonImages (mEditRightButton, "ShiftRight.svg", "", "ShiftRightON.svg", "");
    mImages.setDrawableButtonImages (mEditLeftButton, "ShiftLeft.svg", "", "ShiftLeftON.svg", "");
    mImages.setDrawableButtonImages (mSaveButton, "Save.svg");
    mImages.setDrawableButtonImages (mSuccess, "Success.svg");

    mAllWhiteButton.setTriggeredOnMouseDown (true);
    mAllWhiteButton.onClick = [this]()
    {
        if (mMidiState.getCurrentlyOnInputNotes().size() > 0) { return; }
        mPresetState.handleClickAllWhite();
    };

    mEditLeftButton.setTriggeredOnMouseDown (true);
    mEditLeftButton.onClick = [this]()
    {
        if (mMidiState.getCurrentlyOnInputNotes().size() > 0) { return; }
        mPresetState.handleClickEditLeft();
    };

    mSaveButton.setTriggeredOnMouseDown (true);
    mSaveButton.onClick = [this]() { mPresetState.handleClickSave(); };

    mEditRightButton.setTriggeredOnMouseDown (true);
    mEditRightButton.onClick = [this]()
    {
        if (mMidiState.getCurrentlyOnInputNotes().size() > 0) { return; }
        mPresetState.handleClickEditRight();
    };

    mAllBlackButton.setTriggeredOnMouseDown (true);
    mAllBlackButton.onClick = [this]()
    {
        if (mMidiState.getCurrentlyOnInputNotes().size() > 0) { return; }
        mPresetState.handleClickAllBlack();
    };

    addAndMakeVisible (mAllWhiteButton);
    addAndMakeVisible (mAllBlackButton);
    addAndMakeVisible (mEditRightButton);
    addAndMakeVisible (mEditLeftButton);
    addAndMakeVisible (mSaveButton);
    addChildComponent (mSuccess);
}

EditControlsComponent::~EditControlsComponent()
{
}

//==============================================================================
void EditControlsComponent::resized()
{
    const int buttonSize = roundToInt (ITEM_HEIGHT);
    const int buttonY = roundToInt (SPACE);
    const int shiftGap = 18;
    const int outerGap = 16;
    const int centerX = getWidth() / 2;
    const int saveX = centerX - (buttonSize / 2);
    const int editLeftX = saveX - buttonSize - shiftGap;
    const int editRightX = saveX + buttonSize + shiftGap;
    const int allWhiteX = editLeftX - buttonSize - outerGap;
    const int allBlackX = editRightX + buttonSize + outerGap;

    mAllWhiteButton.setBounds (allWhiteX, buttonY, buttonSize, buttonSize);
    mEditLeftButton.setBounds (editLeftX, buttonY, buttonSize, buttonSize);
    mSaveButton.setBounds (saveX, buttonY, buttonSize, buttonSize);
    mSuccess.setBounds (saveX, buttonY, buttonSize, buttonSize);
    mEditRightButton.setBounds (editRightX, buttonY, buttonSize, buttonSize);
    mAllBlackButton.setBounds (allBlackX, buttonY, buttonSize, buttonSize);
}

//==============================================================================
void EditControlsComponent::handleNewMessage (const DataMessage* inMessage)
{
    switch (inMessage->messageCode)
    {
        case (MessageCode::kToggleMode): { handleToggleMode (inMessage); } break;
        case (MessageCode::kPresetFileNew): { handlePresetFileNew (inMessage); } break;
        case (MessageCode::kPresetModified): { handlePresetModified (inMessage); } break;
        case (MessageCode::kPresetFileSaved): { handlePresetFileSaved (inMessage); } break;
        case (MessageCode::kPresetFileLoaded): { handlePresetFileLoaded (inMessage); } break;
        case (MessageCode::kEditModeOutputNotes): { handlePresetModified (inMessage); } break;
        case (MessageCode::kPresetNameTextChanged): { handlePresetModified (inMessage); } break;
        case (MessageCode::kChordNameTextChanged): { handlePresetModified (inMessage); } break;
        case (MessageCode::kEditModeShiftArrow): { handlePresetModified (inMessage); } break;
        default: { } break;
    };
}

//==============================================================================
void EditControlsComponent::handleToggleMode (const DataMessage* inMessage)
{
    if (mGlobalState.isPlayMode()) { setVisible (false); }

    if (mGlobalState.isEditMode())
    {
        setVisible (true);
        mSuccess.setVisible (false);
        bool isPresetSaveable = mPresetState.isPresetModified() && mPresetState.isPresetValid();
        mImages.setDrawableButtonImages (mSaveButton, isPresetSaveable ? "SaveON.svg" : "Save.svg");
    }
}

void EditControlsComponent::handlePresetFileNew (const DataMessage* inMessage)
{
    mSuccess.setVisible (false);
    mImages.setDrawableButtonImages (mSaveButton, "Save.svg");
}

void EditControlsComponent::handlePresetModified (const DataMessage* inMessage)
{
    mSuccess.setVisible (false);
    mImages.setDrawableButtonImages (mSaveButton, mPresetState.isPresetValid() ? "SaveON.svg" : "Save.svg");
}

void EditControlsComponent::handlePresetFileSaved (const DataMessage* inMessage)
{
    mSuccess.setVisible (true);
    mImages.setDrawableButtonImages (mSaveButton, "Save.svg");
}

void EditControlsComponent::handlePresetFileLoaded (const DataMessage* inMessage)
{
    mSuccess.setVisible (false);
    mImages.setDrawableButtonImages (mSaveButton, "Save.svg");
}
