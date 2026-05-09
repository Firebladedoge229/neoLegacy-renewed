#pragma once

#include "UIScene.h"

class UIScene_SettingsOptionsMenu : public UIScene
{
private:
	enum EControls
	{
		eControl_VerticalSplitscreen,
		eControl_ViewBob,
		eControl_ShowHints,
		eControl_DeathMessages,
		eControl_Autosave,
		eControl_Sensitivity_InGame,
		eControl_Difficulty
	};
protected:
	static int m_iDifficultySettingA[4];
	static int m_iDifficultyTitleSettingA[4];

private:
	UIControl_CheckBox m_checkboxDeathMessages, m_checkboxViewBob, m_checkboxShowHints, m_checkboxVerticalSplitscreen; // Checkboxes
	UIControl_Slider m_sliderAutosave, m_sliderDifficulty, m_sliderSensitivity_InGame; // Sliders
	UIControl_Label m_labelDifficultyText; //Text
	UIControl_Button m_buttonLanguageSelect;

	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_ELEMENT( m_checkboxVerticalSplitscreen, "VerticalSplitscreen")
		UI_MAP_ELEMENT( m_checkboxViewBob, "ViewBob")
		UI_MAP_ELEMENT( m_checkboxShowHints, "ShowHints")
		UI_MAP_ELEMENT( m_checkboxDeathMessages, "DeathMessages")
		// UI_MAP_ELEMENT( m_checkboxMashupWorlds, "ShowMashUpWorlds")
		UI_MAP_ELEMENT( m_sliderAutosave, "Autosave")
		UI_MAP_ELEMENT( m_sliderDifficulty, "Difficulty")
		UI_MAP_ELEMENT( m_sliderSensitivity_InGame, "Sensitivity_InGame")
		UI_MAP_ELEMENT( m_labelDifficultyText, "DifficultyText")
		// UI_MAP_ELEMENT( m_buttonLanguageSelect, "Languages")
	UI_END_MAP_ELEMENTS_AND_NAMES()

	bool m_bNotInGame;
	// bool m_bMashUpWorldsUnhideOption;
	// bool m_bNavigateToLanguageSelector;

public:
	UIScene_SettingsOptionsMenu(int iPad, void *initData, UILayer *parentLayer);
	virtual ~UIScene_SettingsOptionsMenu();

	virtual EUIScene getSceneType() { return eUIScene_SettingsOptionsMenu;}
	
	virtual void tick();

	virtual void updateTooltips();
	virtual void updateComponents();

protected:
	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();

public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);
	virtual void handlePress(F64 controlId, F64 childId);

	virtual void handleReload();

	virtual void handleSliderMove(F64 sliderId, F64 currentValue);

protected:
	void setGameSettings();

};