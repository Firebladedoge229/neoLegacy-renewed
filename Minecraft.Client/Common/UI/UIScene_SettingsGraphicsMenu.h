#pragma once

#include "UIScene.h"
#include "Common/UI/UIControl_CheckBox.h"
#include "Common/UI/UIControl_Slider.h"
#include "Common/UI/UIControl_Button.h"

class UIScene_SettingsGraphicsMenu : public UIScene
{
private:
	enum EControls
	{
		eControl_Clouds,
		eControl_BedrockFog,
		eControl_CustomSkinAnim,
		eControl_VSync,
		eControl_ExclusiveFullscreen,
		eControl_RenderDistance,
		eControl_Gamma,
		eControl_AdvancedOptions,
		eControl_FOV,
		eControl_InterfaceOpacity,
		eControl_Mipmapping,
		eControl_Antialiasing
	};

	UIControl_CheckBox m_checkboxClouds, m_checkboxBedrockFog, m_checkboxCustomSkinAnim, m_checkboxVSync, m_checkboxExclusiveFullscreen; // Checkboxes
	UIControl_Button m_buttonAdvancedOptions;
	UIControl_Slider m_sliderRenderDistance, m_sliderGamma, m_sliderFOV, m_sliderInterfaceOpacity; // Sliders
#ifdef _WINDOWS64
	UIControl_CheckBox m_checkboxNativeMipmapping;
	UIControl_Slider m_sliderNativeFullscreenResolution, m_sliderNativeMaxFramerate, m_sliderNativeAntialiasing;
#endif
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_ELEMENT( m_checkboxClouds, "Clouds")
		UI_MAP_ELEMENT( m_checkboxBedrockFog, "BedrockFog")
		UI_MAP_ELEMENT( m_checkboxCustomSkinAnim, "CustomSkinAnim")
		UI_MAP_ELEMENT( m_checkboxVSync, "VSync")
		UI_MAP_ELEMENT( m_checkboxExclusiveFullscreen, "ExclusiveFullscreen")
		UI_MAP_ELEMENT( m_buttonAdvancedOptions, "DeveloperSettings")
		UI_MAP_ELEMENT( m_sliderRenderDistance, "RenderDistance")
		UI_MAP_ELEMENT( m_sliderGamma, "Gamma")
		UI_MAP_ELEMENT(m_sliderFOV, "FOV")
		UI_MAP_ELEMENT( m_sliderInterfaceOpacity, "InterfaceOpacity")
#ifdef _WINDOWS64
		UI_MAP_ELEMENT( m_sliderNativeFullscreenResolution, "FullscreenResolution")
		UI_MAP_ELEMENT( m_sliderNativeMaxFramerate, "MaxFramerate")
		UI_MAP_ELEMENT( m_checkboxNativeMipmapping, "Mipmapping")
		UI_MAP_ELEMENT( m_sliderNativeAntialiasing, "Antialiasing")
#endif
	UI_END_MAP_ELEMENTS_AND_NAMES()

	bool m_bNotInGame;
	bool m_bAdvancedPage;

#ifdef _WINDOWS64
	void enforceMainPageLayout();
	void enforceAdvancedPageLayout();
	void adjustBackgroundPanelToVisibleControls();
#endif
public:
	UIScene_SettingsGraphicsMenu(int iPad, void *initData, UILayer *parentLayer);
	virtual ~UIScene_SettingsGraphicsMenu();

	virtual EUIScene getSceneType() { return eUIScene_SettingsGraphicsMenu;}
	
	virtual void updateTooltips();
	virtual void updateComponents();

protected:
	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();

public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);
	virtual void handleGainFocus(bool navBack);
	virtual void handlePress(F64 controlId, F64 childId);
	virtual void handleCheckboxToggled(F64 controlId, bool selected);

	virtual void handleSliderMove(F64 sliderId, F64 currentValue);

	static int LevelToDistance(int dist);

	static int DistanceToLevel(int dist);
};
