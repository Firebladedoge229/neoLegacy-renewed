#include "stdafx.h"
#include "UI.h"
#include "UIScene_SettingsGraphicsMenu.h"
#include "../../Minecraft.h"
#include "../../Options.h"
#include "../../GameRenderer.h"

#ifdef _WINDOWS64
extern bool g_bVSync;
extern void SetExclusiveFullscreen(bool enabled);
#endif

namespace
{
	constexpr int FOV_MIN = 70;
	constexpr int FOV_MAX = 110;
	constexpr int FOV_SLIDER_MAX = 100;

#ifdef _WINDOWS64
	constexpr int FULLSCREEN_RESOLUTION_AUTO = 255;
	constexpr int FULLSCREEN_RESOLUTION_SLIDER_MAX = 4;
	constexpr int MAX_FRAMERATE_SLIDER_MAX = 4;
#endif

	int ClampFov(int value)
	{
		if (value < FOV_MIN) return FOV_MIN;
		if (value > FOV_MAX) return FOV_MAX;
		return value;
	}

	[[maybe_unused]]
	int FovToSliderValue(float fov)
	{
		const int clampedFov = ClampFov(static_cast<int>(fov + 0.5f));
		return ((clampedFov - FOV_MIN) * FOV_SLIDER_MAX) / (FOV_MAX - FOV_MIN);
	}

	int sliderValueToFov(int sliderValue)
	{
		if (sliderValue < 0) sliderValue = 0;
		if (sliderValue > FOV_SLIDER_MAX) sliderValue = FOV_SLIDER_MAX;
		return FOV_MIN + ((sliderValue * (FOV_MAX - FOV_MIN)) / FOV_SLIDER_MAX);
	}

#ifdef _WINDOWS64
	int ClampFullscreenResolutionSetting(int setting)
	{
		if (setting == FULLSCREEN_RESOLUTION_AUTO)
			return FULLSCREEN_RESOLUTION_AUTO;
		if (setting < 0 || setting > 3)
			return FULLSCREEN_RESOLUTION_AUTO;
		return setting;
	}

	int FullscreenResolutionSettingToSlider(int setting)
	{
		switch (ClampFullscreenResolutionSetting(setting))
		{
		case 3: return 0;
		case 2: return 1;
		case 1: return 2;
		case 0: return 3;
		default: return 4;
		}
	}

	unsigned char SliderToFullscreenResolutionSetting(int sliderValue)
	{
		switch (sliderValue)
		{
		case 0: return 3;
		case 1: return 2;
		case 2: return 1;
		case 3: return 0;
		default: return FULLSCREEN_RESOLUTION_AUTO;
		}
	}

	const wchar_t* GetResolutionLabel(int sliderValue)
	{
		switch (sliderValue)
		{
		case 0: return L"1024x768";
		case 1: return L"1280x720";
		case 2: return L"1600x900";
		case 3: return L"1920x1080";
		default: return L"Native";
		}
	}

	int ClampMaxFramerateSetting(int setting)
	{
		if (setting < 0 || setting > MAX_FRAMERATE_SLIDER_MAX)
			return 0;
		return setting;
	}

	int MaxFramerateSettingToSlider(int setting)
	{
		switch (ClampMaxFramerateSetting(setting))
		{
		case 4: return 0;
		case 3: return 1;
		case 2: return 2;
		case 1: return 3;
		default: return 4;
		}
	}

	unsigned char SliderToMaxFramerateSetting(int sliderValue)
	{
		switch (sliderValue)
		{
		case 0: return 4;
		case 1: return 3;
		case 2: return 2;
		case 3: return 1;
		default: return 0;
		}
	}

	const wchar_t* GetMaxFramerateLabel(int sliderValue)
	{
		switch (sliderValue)
		{
		case 0: return L"30 FPS";
		case 1: return L"35 FPS";
		case 2: return L"60 FPS";
		case 3: return L"120 FPS";
		default: return L"Unlimited";
		}
	}

	void SetNamedControlProperty(IggyValuePath* parentPath, const char* childName, IggyName propertyName, F64 value)
	{
		if(parentPath == nullptr)
		{
			return;
		}

		IggyValuePath childPath;
		if(IggyValuePathMakeNameRef(&childPath, parentPath, childName))
		{
			IggyValueSetF64RS(&childPath, propertyName, nullptr, value);
		}
	}

	void SetBackgroundPanelProperty(UIScene* scene, IggyName propertyName, F64 value)
	{
		if(scene == nullptr || scene->getMovie() == nullptr)
		{
			return;
		}

		IggyValuePath* rootPath = IggyPlayerRootPath(scene->getMovie());
		if(rootPath == nullptr)
		{
			return;
		}

		SetNamedControlProperty(rootPath, "BackgroundPanel", propertyName, value);

		IggyValuePath mainPanelPath;
		if(IggyValuePathMakeNameRef(&mainPanelPath, rootPath, "MainPanel"))
		{
			SetNamedControlProperty(&mainPanelPath, "BackgroundPanel", propertyName, value);
		}
	}
#endif
}

int UIScene_SettingsGraphicsMenu::LevelToDistance(int level)
{
	static const int table[6] = {2,4,8,16,32,64};
	if(level < 0) level = 0;
	if(level > 5) level = 5;
	return table[level];
}

int UIScene_SettingsGraphicsMenu::DistanceToLevel(int dist)
{
	static const int table[6] = {2,4,8,16,32,64};
	for(int i = 0; i < 6; i++)
	{
		if(table[i] == dist)
			return i;
	}
	return 3;
}

UIScene_SettingsGraphicsMenu::UIScene_SettingsGraphicsMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	m_bAdvancedPage = (initData != nullptr);

	// Setup all the Iggy references we need for this scene
	initialiseMovie();

	m_bNotInGame=(Minecraft::GetInstance()->level==nullptr);
	const bool bInGame=(Minecraft::GetInstance()->level!=nullptr);

	#ifdef _WINDOWS64
	if(m_bAdvancedPage)
	{
		m_checkboxClouds.init(L"Antialiasing", eControl_Antialiasing, (app.GetGameSettings(m_iPad, eGameSetting_Antialiasing) != 0));
		m_checkboxBedrockFog.init(L"Mipmapping", eControl_Mipmapping, (app.GetGameSettings(m_iPad, eGameSetting_Mipmapping) != 0));
	}
	else
	#endif
	{
		m_checkboxClouds.init(app.GetString(IDS_CHECKBOX_RENDER_CLOUDS),eControl_Clouds,(app.GetGameSettings(m_iPad,eGameSetting_Clouds)!=0));
		m_checkboxBedrockFog.init(app.GetString(IDS_CHECKBOX_RENDER_BEDROCKFOG),eControl_BedrockFog,(app.GetGameSettings(m_iPad,eGameSetting_BedrockFog)!=0));
	}
	m_checkboxCustomSkinAnim.init(app.GetString(IDS_CHECKBOX_CUSTOM_SKIN_ANIM),eControl_CustomSkinAnim,(app.GetGameSettings(m_iPad,eGameSetting_CustomSkinAnim)!=0));
	m_checkboxVSync.init(L"VSync",eControl_VSync,(app.GetGameSettings(m_iPad,eGameSetting_VSync)!=0));
	m_checkboxExclusiveFullscreen.init(L"Fullscreen",eControl_ExclusiveFullscreen,(app.GetGameSettings(m_iPad,eGameSetting_ExclusiveFullscreen)!=0));
	m_buttonAdvancedOptions.init(L"Advanced Options", eControl_AdvancedOptions);

	WCHAR TempString[256];

	#ifdef _WINDOWS64
	if(m_bAdvancedPage)
	{
		const int initialResolutionSlider = FullscreenResolutionSettingToSlider(app.GetGameSettings(m_iPad, eGameSetting_FullscreenResolution));
		swprintf(TempString, 256, L"Resolution: %ls", GetResolutionLabel(initialResolutionSlider));
		m_sliderRenderDistance.init(TempString, eControl_RenderDistance, 0, FULLSCREEN_RESOLUTION_SLIDER_MAX, initialResolutionSlider);

		const int initialMaxFramerateSlider = MaxFramerateSettingToSlider(app.GetGameSettings(m_iPad, eGameSetting_MaxFramerate));
		swprintf(TempString, 256, L"Max Framerate: %ls", GetMaxFramerateLabel(initialMaxFramerateSlider));
		m_sliderGamma.init(TempString, eControl_Gamma, 0, MAX_FRAMERATE_SLIDER_MAX, initialMaxFramerateSlider);
	}
	else
	#endif
	{
		swprintf(TempString, 256, L"Render Distance: %d",app.GetGameSettings(m_iPad,eGameSetting_RenderDistance));
		m_sliderRenderDistance.init(TempString,eControl_RenderDistance,0,3,DistanceToLevel(app.GetGameSettings(m_iPad,eGameSetting_RenderDistance)));

		swprintf(TempString, 256, L"%ls: %d%%", app.GetString(IDS_SLIDER_GAMMA),app.GetGameSettings(m_iPad,eGameSetting_Gamma));
		m_sliderGamma.init(TempString,eControl_Gamma,0,100,app.GetGameSettings(m_iPad,eGameSetting_Gamma));
	}

	const int initialFovSlider = app.GetGameSettings(m_iPad, eGameSetting_FOV);
	const int initialFovDeg = sliderValueToFov(initialFovSlider);
	swprintf(TempString, 256, L"FOV: %d", initialFovDeg);
	m_sliderFOV.init(TempString, eControl_FOV, 0, FOV_SLIDER_MAX, initialFovSlider);

	swprintf(TempString, 256, L"%ls: %d%%", app.GetString(IDS_SLIDER_INTERFACEOPACITY),app.GetGameSettings(m_iPad,eGameSetting_InterfaceOpacity));
	m_sliderInterfaceOpacity.init(TempString,eControl_InterfaceOpacity,0,100,app.GetGameSettings(m_iPad,eGameSetting_InterfaceOpacity));

#ifdef _WINDOWS64
	m_sliderNativeFullscreenResolution.setVisible(false);
	removeControl(&m_sliderNativeFullscreenResolution, false);
	m_sliderNativeMaxFramerate.setVisible(false);
	removeControl(&m_sliderNativeMaxFramerate, false);
	m_checkboxNativeMipmapping.setVisible(false);
	removeControl(&m_checkboxNativeMipmapping, false);
	m_sliderNativeAntialiasing.setVisible(false);
	removeControl(&m_sliderNativeAntialiasing, false);
#endif

	if(m_bAdvancedPage)
	{
		m_checkboxCustomSkinAnim.setVisible(false);
		removeControl(&m_checkboxCustomSkinAnim, false);
		m_checkboxVSync.setVisible(false);
		removeControl(&m_checkboxVSync, false);
		m_checkboxExclusiveFullscreen.setVisible(false);
		removeControl(&m_checkboxExclusiveFullscreen, false);
		m_buttonAdvancedOptions.setVisible(false);
		removeControl(&m_buttonAdvancedOptions, false);
		m_sliderInterfaceOpacity.setVisible(false);
		removeControl(&m_sliderInterfaceOpacity, false);
		m_sliderFOV.setVisible(false);
		removeControl(&m_sliderFOV, false);
	}
	else
	{
		#ifndef _WINDOWS64
		m_sliderFOV.setVisible(false);
		removeControl(&m_sliderFOV, true);
		m_sliderInterfaceOpacity.setVisible(false);
		removeControl(&m_sliderInterfaceOpacity, true);
		#endif
	}

#ifndef _WINDOWS64
	removeControl(&m_checkboxVSync, true);
	removeControl(&m_checkboxExclusiveFullscreen, true);
#endif

	if(bInGame)
	{
#ifndef _WINDOWS64
		if(!m_bAdvancedPage)
		{
			const bool bIsPrimaryPad=(ProfileManager.GetPrimaryPad()==m_iPad);
			if(bIsPrimaryPad)
			{
				if(!g_NetworkManager.IsHost())
				{
					removeControl(&m_checkboxBedrockFog, true);
				}
			}
			else
			{
				removeControl(&m_checkboxBedrockFog, true);
				removeControl(&m_checkboxCustomSkinAnim, true);
				removeControl(&m_buttonAdvancedOptions, true);
			}
		}
#endif
	}

#ifdef _WINDOWS64
	{
		IggyName navDown = registerFastName(L"m_objNavDown");
		IggyName navUp   = registerFastName(L"m_objNavUp");

		if(m_bAdvancedPage)
		{
			IggyValueSetStringUTF8RS(m_checkboxClouds.getIggyValuePath(), navDown, nullptr, "BedrockFog", -1);

			IggyValueSetStringUTF8RS(m_checkboxBedrockFog.getIggyValuePath(), navUp, nullptr, "Clouds", -1);
			IggyValueSetStringUTF8RS(m_checkboxBedrockFog.getIggyValuePath(), navDown, nullptr, "RenderDistance", -1);

			IggyValueSetStringUTF8RS(m_sliderRenderDistance.getIggyValuePath(), navUp, nullptr, "BedrockFog", -1);
			IggyValueSetStringUTF8RS(m_sliderRenderDistance.getIggyValuePath(), navDown, nullptr, "Gamma", -1);

			IggyValueSetStringUTF8RS(m_sliderGamma.getIggyValuePath(), navUp, nullptr, "RenderDistance", -1);
		}
		else
		{
			IggyValueSetStringUTF8RS(m_checkboxClouds.getIggyValuePath(), navDown, nullptr, "BedrockFog", -1);

			IggyValueSetStringUTF8RS(m_checkboxBedrockFog.getIggyValuePath(), navUp, nullptr, "Clouds", -1);
			IggyValueSetStringUTF8RS(m_checkboxBedrockFog.getIggyValuePath(), navDown, nullptr, "CustomSkinAnim", -1);

			IggyValueSetStringUTF8RS(m_checkboxCustomSkinAnim.getIggyValuePath(), navUp, nullptr, "BedrockFog", -1);
			IggyValueSetStringUTF8RS(m_checkboxCustomSkinAnim.getIggyValuePath(), navDown, nullptr, "VSync", -1);

			IggyValueSetStringUTF8RS(m_checkboxVSync.getIggyValuePath(), navUp, nullptr, "CustomSkinAnim", -1);
			IggyValueSetStringUTF8RS(m_checkboxVSync.getIggyValuePath(), navDown, nullptr, "ExclusiveFullscreen", -1);

			IggyValueSetStringUTF8RS(m_checkboxExclusiveFullscreen.getIggyValuePath(), navUp, nullptr, "VSync", -1);
			IggyValueSetStringUTF8RS(m_checkboxExclusiveFullscreen.getIggyValuePath(), navDown, nullptr, "RenderDistance", -1);
			IggyValueSetStringUTF8RS(m_sliderRenderDistance.getIggyValuePath(), navUp, nullptr, "ExclusiveFullscreen", -1);
			IggyValueSetStringUTF8RS(m_sliderRenderDistance.getIggyValuePath(), navDown, nullptr, "Gamma", -1);
			IggyValueSetStringUTF8RS(m_sliderGamma.getIggyValuePath(), navUp, nullptr, "RenderDistance", -1);

			IggyValueSetStringUTF8RS(m_sliderGamma.getIggyValuePath(), navDown, nullptr, "FOV", -1);
			IggyValueSetStringUTF8RS(m_sliderFOV.getIggyValuePath(), navUp, nullptr, "Gamma", -1);
			IggyValueSetStringUTF8RS(m_sliderFOV.getIggyValuePath(), navDown, nullptr, "InterfaceOpacity", -1);
			IggyValueSetStringUTF8RS(m_sliderInterfaceOpacity.getIggyValuePath(), navUp, nullptr, "FOV", -1);
			IggyValueSetStringUTF8RS(m_sliderInterfaceOpacity.getIggyValuePath(), navDown, nullptr, "DeveloperSettings", -1);

			IggyValueSetStringUTF8RS(m_buttonAdvancedOptions.getIggyValuePath(), navUp, nullptr, "InterfaceOpacity", -1);
		}
	}
#endif

	doHorizontalResizeCheck();

#ifdef _WINDOWS64
	if(m_bAdvancedPage)
	{
		enforceAdvancedPageLayout();
	}
	else
	{
		enforceMainPageLayout();
	}
#endif

	if(app.GetLocalPlayerCount()>1)
	{
#if TO_BE_IMPLEMENTED
		app.AdjustSplitscreenScene(m_hObj,&m_OriginalPosition,m_iPad);
#endif
	}
}

UIScene_SettingsGraphicsMenu::~UIScene_SettingsGraphicsMenu()
{
}

wstring UIScene_SettingsGraphicsMenu::getMoviePath()
{
	if(app.GetLocalPlayerCount() > 1)
	{
		return L"SettingsGraphicsMenuSplit";
	}
	else
	{
		return L"SettingsGraphicsMenu";
	}
}

void UIScene_SettingsGraphicsMenu::updateTooltips()
{
	ui.SetTooltips( m_iPad, IDS_TOOLTIPS_SELECT,IDS_TOOLTIPS_BACK);
}

void UIScene_SettingsGraphicsMenu::updateComponents()
{
	const bool bNotInGame=(Minecraft::GetInstance()->level==nullptr);
	if(bNotInGame)
	{
		m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,true);
		m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
	}
	else
	{
		m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,false);

		if( app.GetLocalPlayerCount() == 1 ) m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
		else m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,false);

	}
}

#ifdef _WINDOWS64
void UIScene_SettingsGraphicsMenu::adjustBackgroundPanelToVisibleControls()
{
	int top = INT_MAX;
	int bottom = INT_MIN;

	auto includeBounds = [&](UIControl& control)
	{
		if(control.getHidden() || !control.getVisible())
		{
			return;
		}

		control.UpdateControl();
		const int y = control.getYPos();
		const int h = control.getHeight();
		if(h <= 0)
		{
			return;
		}

		if(y < top)
		{
			top = y;
		}
		if((y + h) > bottom)
		{
			bottom = y + h;
		}
	};

	if(m_bAdvancedPage)
	{
		includeBounds(m_checkboxClouds);
		includeBounds(m_checkboxBedrockFog);
		includeBounds(m_sliderRenderDistance);
		includeBounds(m_sliderGamma);
	}
	else
	{
		includeBounds(m_checkboxClouds);
		includeBounds(m_checkboxBedrockFog);
		includeBounds(m_checkboxCustomSkinAnim);
		includeBounds(m_checkboxVSync);
		includeBounds(m_checkboxExclusiveFullscreen);
		includeBounds(m_sliderRenderDistance);
		includeBounds(m_sliderGamma);
		includeBounds(m_sliderFOV);
		includeBounds(m_sliderInterfaceOpacity);
		includeBounds(m_buttonAdvancedOptions);
	}

	if(top == INT_MAX || bottom <= top)
	{
		return;
	}

	IggyName nameY = registerFastName(L"y");
	IggyName nameHeight = registerFastName(L"height");
	const F64 panelY = static_cast<F64>(top - 16);
	const F64 panelHeight = static_cast<F64>((bottom - top) + 32);
	SetBackgroundPanelProperty(this, nameY, panelY);
	SetBackgroundPanelProperty(this, nameHeight, panelHeight);
}

void UIScene_SettingsGraphicsMenu::enforceMainPageLayout()
{
	if(m_bAdvancedPage || m_buttonAdvancedOptions.getHidden() || !m_buttonAdvancedOptions.getVisible())
	{
		return;
	}

	UIControl_Slider* anchorSlider = &m_sliderInterfaceOpacity;
	if(anchorSlider->getHidden() || !anchorSlider->getVisible())
	{
		anchorSlider = &m_sliderFOV;
	}

	anchorSlider->UpdateControl();
	m_buttonAdvancedOptions.UpdateControl();

	const F64 sliderX = static_cast<F64>(anchorSlider->getXPos());
	F64 sliderWidth = static_cast<F64>(anchorSlider->GetRealWidth());
	if(sliderWidth <= 0.0)
	{
		sliderWidth = static_cast<F64>(anchorSlider->getWidth());
	}
	const F64 buttonY = static_cast<F64>(anchorSlider->getYPos() + anchorSlider->getHeight() + 12);

	IggyName nameX = registerFastName(L"x");
	IggyName nameY = registerFastName(L"y");
	IggyName nameWidth = registerFastName(L"width");
	IggyValueSetF64RS(m_buttonAdvancedOptions.getIggyValuePath(), nameX, nullptr, sliderX);
	IggyValueSetF64RS(m_buttonAdvancedOptions.getIggyValuePath(), nameY, nullptr, buttonY);
	IggyValueSetF64RS(m_buttonAdvancedOptions.getIggyValuePath(), nameWidth, nullptr, sliderWidth);

	adjustBackgroundPanelToVisibleControls();
}

void UIScene_SettingsGraphicsMenu::enforceAdvancedPageLayout()
{
	if(!m_bAdvancedPage)
	{
		return;
	}

	m_checkboxClouds.UpdateControl();
	m_checkboxBedrockFog.UpdateControl();
	m_sliderRenderDistance.UpdateControl();
	m_sliderGamma.UpdateControl();

	IggyName nameY = registerFastName(L"y");
	F64 nextY = static_cast<F64>(m_checkboxClouds.getYPos());
	IggyValueSetF64RS(m_checkboxClouds.getIggyValuePath(), nameY, nullptr, nextY);

	nextY += static_cast<F64>(m_checkboxClouds.getHeight() + 10);
	IggyValueSetF64RS(m_checkboxBedrockFog.getIggyValuePath(), nameY, nullptr, nextY);

	nextY += static_cast<F64>(m_checkboxBedrockFog.getHeight() + 14);
	IggyValueSetF64RS(m_sliderRenderDistance.getIggyValuePath(), nameY, nullptr, nextY);

	nextY += static_cast<F64>(m_sliderRenderDistance.getHeight() + 12);
	IggyValueSetF64RS(m_sliderGamma.getIggyValuePath(), nameY, nullptr, nextY);

	adjustBackgroundPanelToVisibleControls();
}
#endif

void UIScene_SettingsGraphicsMenu::handleGainFocus(bool navBack)
{
	UIScene::handleGainFocus(navBack);

#ifdef _WINDOWS64
	if(navBack && !m_bAdvancedPage)
	{
		m_buttonAdvancedOptions.init(L"Advanced Options", eControl_AdvancedOptions);
		doHorizontalResizeCheck();
		enforceMainPageLayout();
	}
#endif
}

void UIScene_SettingsGraphicsMenu::handleCheckboxToggled(F64 controlId, bool selected)
{
#ifdef _WINDOWS64
	if(!m_bAdvancedPage)
	{
		return;
	}

	switch(static_cast<int>(controlId))
	{
	case eControl_Antialiasing:
		app.SetGameSettings(m_iPad, eGameSetting_Antialiasing, selected ? 1 : 0);
		break;
	case eControl_Mipmapping:
		app.SetGameSettings(m_iPad, eGameSetting_Mipmapping, selected ? 1 : 0);
		break;
	}
#else
	(void)controlId;
	(void)selected;
#endif
}

void UIScene_SettingsGraphicsMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	ui.AnimateKeyPress(iPad, key, repeat, pressed, released);
	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed)
		{
			if(!m_bAdvancedPage)
			{
				app.SetGameSettings(m_iPad,eGameSetting_Clouds,m_checkboxClouds.IsChecked()?1:0);
				app.SetGameSettings(m_iPad,eGameSetting_BedrockFog,m_checkboxBedrockFog.IsChecked()?1:0);
				app.SetGameSettings(m_iPad,eGameSetting_CustomSkinAnim,m_checkboxCustomSkinAnim.IsChecked()?1:0);
				app.SetGameSettings(m_iPad,eGameSetting_VSync,m_checkboxVSync.IsChecked()?1:0);
				app.SetGameSettings(m_iPad,eGameSetting_ExclusiveFullscreen,m_checkboxExclusiveFullscreen.IsChecked()?1:0);

#ifdef _WINDOWS64
				g_bVSync = m_checkboxVSync.IsChecked();
				SetExclusiveFullscreen(m_checkboxExclusiveFullscreen.IsChecked());
#endif
			}
			else
			{
#ifdef _WINDOWS64
				app.SetGameSettings(m_iPad, eGameSetting_Antialiasing, m_checkboxClouds.IsChecked() ? 1 : 0);
				app.SetGameSettings(m_iPad, eGameSetting_Mipmapping, m_checkboxBedrockFog.IsChecked() ? 1 : 0);
				app.SetGameSettings(m_iPad, eGameSetting_FullscreenResolution, SliderToFullscreenResolutionSetting(m_sliderRenderDistance.getCurrentValue()));
				app.SetGameSettings(m_iPad, eGameSetting_MaxFramerate, SliderToMaxFramerateSetting(m_sliderGamma.getCurrentValue()));
#endif
			}
			navigateBack();
			handled = true;
		}
		break;
	case ACTION_MENU_OK:
#ifdef __ORBIS__
	case ACTION_MENU_TOUCHPAD_PRESS:
#endif
		if(pressed && !m_bAdvancedPage && m_buttonAdvancedOptions.hasFocus())
		{
			ui.PlayUISFX(eSFX_Press);
			ui.NavigateToScene(m_iPad, eUIScene_SettingsGraphicsMenu, reinterpret_cast<void*>(1));
			handled = true;
			break;
		}
		sendInputToMovie(key, repeat, pressed, released);
		break;
	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
	case ACTION_MENU_LEFT:
	case ACTION_MENU_RIGHT:
		sendInputToMovie(key, repeat, pressed, released);
		break;
	}
}

void UIScene_SettingsGraphicsMenu::handlePress(F64 controlId, F64 childId)
{
	ui.PlayUISFX(eSFX_Press);
	if(static_cast<int>(controlId) == eControl_AdvancedOptions)
	{
		ui.NavigateToScene(m_iPad, eUIScene_SettingsGraphicsMenu, reinterpret_cast<void*>(1));
	}
}

void UIScene_SettingsGraphicsMenu::handleSliderMove(F64 sliderId, F64 currentValue)
{
	WCHAR TempString[256];
	const int value = static_cast<int>(currentValue);
	switch(static_cast<int>(sliderId))
	{
	case eControl_RenderDistance:
		{
		#ifdef _WINDOWS64
			if(m_bAdvancedPage)
			{
				int sliderValue = value;
				if(sliderValue < 0) sliderValue = 0;
				if(sliderValue > FULLSCREEN_RESOLUTION_SLIDER_MAX) sliderValue = FULLSCREEN_RESOLUTION_SLIDER_MAX;

				m_sliderRenderDistance.handleSliderMove(sliderValue);
				app.SetGameSettings(m_iPad, eGameSetting_FullscreenResolution, SliderToFullscreenResolutionSetting(sliderValue));
				swprintf(TempString, 256, L"Resolution: %ls", GetResolutionLabel(sliderValue));
				m_sliderRenderDistance.setLabel(TempString);
				break;
			}
		#endif

			m_sliderRenderDistance.handleSliderMove(value);
			const int dist = LevelToDistance(value);

			app.SetGameSettings(m_iPad,eGameSetting_RenderDistance,dist);

			const Minecraft* mc = Minecraft::GetInstance();
			mc->options->viewDistance = 3 - value;
			swprintf(TempString,256,L"Render Distance: %d",dist);
			m_sliderRenderDistance.setLabel(TempString);
		}
		break;

	case eControl_Gamma:
		#ifdef _WINDOWS64
			if(m_bAdvancedPage)
			{
				int sliderValue = value;
				if(sliderValue < 0) sliderValue = 0;
				if(sliderValue > MAX_FRAMERATE_SLIDER_MAX) sliderValue = MAX_FRAMERATE_SLIDER_MAX;

				m_sliderGamma.handleSliderMove(sliderValue);
				app.SetGameSettings(m_iPad, eGameSetting_MaxFramerate, SliderToMaxFramerateSetting(sliderValue));
				swprintf(TempString, 256, L"Max Framerate: %ls", GetMaxFramerateLabel(sliderValue));
				m_sliderGamma.setLabel(TempString);
				break;
			}
		#endif

		m_sliderGamma.handleSliderMove(value);
		app.SetGameSettings(m_iPad,eGameSetting_Gamma,value);
		swprintf(TempString, 256, L"%ls: %d%%", app.GetString(IDS_SLIDER_GAMMA), value);
		m_sliderGamma.setLabel(TempString);
		break;

	case eControl_FOV:
		{
			m_sliderFOV.handleSliderMove(value);
			const Minecraft* pMinecraft = Minecraft::GetInstance();
			const int fovValue = sliderValueToFov(value);
			pMinecraft->gameRenderer->SetFovVal(static_cast<float>(fovValue));
			app.SetGameSettings(m_iPad, eGameSetting_FOV, value);
			swprintf(TempString, 256, L"FOV: %d", fovValue);
			m_sliderFOV.setLabel(TempString);
		}
		break;

	case eControl_InterfaceOpacity:
		m_sliderInterfaceOpacity.handleSliderMove(value);
		app.SetGameSettings(m_iPad,eGameSetting_InterfaceOpacity,value);
		swprintf(TempString, 256, L"%ls: %d%%", app.GetString(IDS_SLIDER_INTERFACEOPACITY), value);
		m_sliderInterfaceOpacity.setLabel(TempString);
		break;
	}
}
