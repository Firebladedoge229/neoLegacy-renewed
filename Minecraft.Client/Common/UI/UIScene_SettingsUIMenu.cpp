#include "stdafx.h"
#include "UI.h"
#include "UIScene_SettingsUIMenu.h"

namespace
{
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
}

void UIScene_SettingsUIMenu::adjustBackgroundPanelToVisibleControls()
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

	includeBounds(m_checkboxDisplayHUD);
	includeBounds(m_checkboxDisplayHand);
	includeBounds(m_checkboxDisplayDeathMessages);
	includeBounds(m_checkboxDisplayAnimatedCharacter);
	includeBounds(m_checkboxSplitscreen);
	includeBounds(m_checkboxShowSplitscreenGamertags);
	includeBounds(m_sliderUISize);
	includeBounds(m_sliderUISizeSplitscreen);
	includeBounds(m_checkboxDeveloperSettings);

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

UIScene_SettingsUIMenu::UIScene_SettingsUIMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();

	m_bNotInGame=(Minecraft::GetInstance()->level==nullptr);

	m_checkboxDisplayHUD.init(app.GetString(IDS_CHECKBOX_DISPLAY_HUD),eControl_DisplayHUD,(app.GetGameSettings(m_iPad,eGameSetting_DisplayHUD)!=0));
	m_checkboxDisplayHand.init(app.GetString(IDS_CHECKBOX_DISPLAY_HAND),eControl_DisplayHand,(app.GetGameSettings(m_iPad,eGameSetting_DisplayHand)!=0));
	m_checkboxDisplayDeathMessages.init(app.GetString(IDS_CHECKBOX_DEATH_MESSAGES),eControl_DisplayDeathMessages,(app.GetGameSettings(m_iPad,eGameSetting_DeathMessages)!=0));
	m_checkboxDisplayAnimatedCharacter.init(app.GetString(IDS_CHECKBOX_ANIMATED_CHARACTER),eControl_DisplayAnimatedCharacter,(app.GetGameSettings(m_iPad,eGameSetting_AnimatedCharacter)!=0));
	m_checkboxDeveloperSettings.init(L"Developer Settings", eControl_DeveloperSettings, (app.GetGameSettings(m_iPad, eGameSetting_DeveloperSettings) != 0));
	m_checkboxSplitscreen.init(app.GetString(IDS_CHECKBOX_VERTICAL_SPLIT_SCREEN),eControl_Splitscreen,(app.GetGameSettings(m_iPad,eGameSetting_SplitScreenVertical)!=0));
	m_checkboxShowSplitscreenGamertags.init(app.GetString(IDS_CHECKBOX_DISPLAY_SPLITSCREENGAMERTAGS),eControl_ShowSplitscreenGamertags,(app.GetGameSettings(m_iPad,eGameSetting_DisplaySplitscreenGamertags)!=0));

	WCHAR TempString[256];

	swprintf( (WCHAR *)TempString, 256, L"%ls: %d", app.GetString( IDS_SLIDER_UISIZE ),app.GetGameSettings(m_iPad,eGameSetting_UISize)+1);	
	m_sliderUISize.init(TempString,eControl_UISize,1,3,app.GetGameSettings(m_iPad,eGameSetting_UISize)+1);

	swprintf( (WCHAR *)TempString, 256, L"%ls: %d", app.GetString( IDS_SLIDER_UISIZESPLITSCREEN ),app.GetGameSettings(m_iPad,eGameSetting_UISizeSplitscreen)+1);	
	m_sliderUISizeSplitscreen.init(TempString,eControl_UISizeSplitscreen,1,3,app.GetGameSettings(m_iPad,eGameSetting_UISizeSplitscreen)+1);

	bool bInGame=(Minecraft::GetInstance()->level!=nullptr);
	bool bPrimaryPlayer = ProfileManager.GetPrimaryPad()==m_iPad;

	// if we're not in the game, we need to use basescene 0 
	if(bInGame)
	{
		// If the game has started, then you need to be the host to change the in-game gamertags
		if(!bPrimaryPlayer)
		{	
			// hide things we don't want the splitscreen player changing
			removeControl(&m_checkboxSplitscreen, true);
			removeControl(&m_checkboxShowSplitscreenGamertags, true);
			removeControl(&m_checkboxDeveloperSettings, true);
		}
	}

	if(bPrimaryPlayer)
	{
		IggyName navDown = registerFastName(L"m_objNavDown");
		IggyName navUp = registerFastName(L"m_objNavUp");
		IggyValueSetStringUTF8RS(m_checkboxDisplayAnimatedCharacter.getIggyValuePath(), navDown, nullptr, "Splitscreen", -1);
		IggyValueSetStringUTF8RS(m_checkboxShowSplitscreenGamertags.getIggyValuePath(), navDown, nullptr, "UISize", -1);
		IggyValueSetStringUTF8RS(m_sliderUISize.getIggyValuePath(), navDown, nullptr, "UISizeSplitscreen", -1);
		IggyValueSetStringUTF8RS(m_sliderUISizeSplitscreen.getIggyValuePath(), navUp, nullptr, "UISize", -1);
		IggyValueSetStringUTF8RS(m_sliderUISizeSplitscreen.getIggyValuePath(), navDown, nullptr, "DeveloperSettings", -1);
		IggyValueSetStringUTF8RS(m_checkboxDeveloperSettings.getIggyValuePath(), navUp, nullptr, "UISizeSplitscreen", -1);
		IggyValueSetStringUTF8RS(m_checkboxDeveloperSettings.getIggyValuePath(), navDown, nullptr, "Splitscreen", -1);
		IggyValueSetStringUTF8RS(m_checkboxSplitscreen.getIggyValuePath(), navUp, nullptr, "DeveloperSettings", -1);

		m_sliderUISizeSplitscreen.UpdateControl();
		IggyName nameY = registerFastName(L"y");
		const F64 newDeveloperY = static_cast<F64>(m_sliderUISizeSplitscreen.getYPos() + m_sliderUISizeSplitscreen.getHeight() + 8);
		IggyValueSetF64RS(m_checkboxDeveloperSettings.getIggyValuePath(), nameY, nullptr, newDeveloperY);
	}


	if(app.GetLocalPlayerCount()>1)
	{
#if TO_BE_IMPLEMENTED
		app.AdjustSplitscreenScene(m_hObj,&m_OriginalPosition,m_iPad);
#endif
	}

	doHorizontalResizeCheck();
	adjustBackgroundPanelToVisibleControls();
}

void UIScene_SettingsUIMenu::updateTooltips()
{
	ui.SetTooltips( m_iPad, IDS_TOOLTIPS_SELECT,IDS_TOOLTIPS_BACK);
}

void UIScene_SettingsUIMenu::updateComponents()
{
	bool bNotInGame=(Minecraft::GetInstance()->level==nullptr);
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

UIScene_SettingsUIMenu::~UIScene_SettingsUIMenu()
{
}

wstring UIScene_SettingsUIMenu::getMoviePath()
{
	if(app.GetLocalPlayerCount() > 1)
	{
		return L"SettingsUIMenuSplit";
	}
	else
	{
		return L"SettingsUIMenu";
	}
}

void UIScene_SettingsUIMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	ui.AnimateKeyPress(iPad, key, repeat, pressed, released);

	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed)
		{
			// check the checkboxes
			app.SetGameSettings(m_iPad,eGameSetting_DisplayHUD,m_checkboxDisplayHUD.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_DisplayHand,m_checkboxDisplayHand.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_DisplaySplitscreenGamertags,m_checkboxShowSplitscreenGamertags.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_DeathMessages,m_checkboxDisplayDeathMessages.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_AnimatedCharacter,m_checkboxDisplayAnimatedCharacter.IsChecked()?1:0);
			if(ProfileManager.GetPrimaryPad()==m_iPad)
			{
				app.SetGameSettings(m_iPad,eGameSetting_DeveloperSettings,m_checkboxDeveloperSettings.IsChecked()?1:0);
			}

			// if the splitscreen vertical/horizontal has changed, need to update the scenes
			if(app.GetGameSettings(m_iPad,eGameSetting_SplitScreenVertical)!=(m_checkboxSplitscreen.IsChecked()?1:0))
			{
				// changed
				app.SetGameSettings(m_iPad,eGameSetting_SplitScreenVertical,m_checkboxSplitscreen.IsChecked()?1:0);

				// close the xui scenes, so we don't have the navigate backed to menu at the wrong place
				if(app.GetLocalPlayerCount()==2)
				{
					ui.CloseAllPlayersScenes();
				}
				else
				{
					navigateBack();
				}
			}
			else
			{
				navigateBack();
			}
			handled = true;
		}
		break;
	case ACTION_MENU_OK:
#ifdef __ORBIS__
	case ACTION_MENU_TOUCHPAD_PRESS:
#endif
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

void UIScene_SettingsUIMenu::handleCheckboxToggled(F64 controlId, bool selected)
{
	if(static_cast<int>(controlId) == eControl_DeveloperSettings && ProfileManager.GetPrimaryPad() == m_iPad)
	{
		app.SetGameSettings(m_iPad, eGameSetting_DeveloperSettings, selected ? 1 : 0);
	}
}

void UIScene_SettingsUIMenu::handleSliderMove(F64 sliderId, F64 currentValue)
{
	WCHAR TempString[256];
	int value = static_cast<int>(currentValue);
	switch(static_cast<int>(sliderId))
	{
	case eControl_UISize:
		m_sliderUISize.handleSliderMove(value);

		swprintf( (WCHAR *)TempString, 256, L"%ls: %d", app.GetString( IDS_SLIDER_UISIZE ),value);		
		m_sliderUISize.setLabel(TempString);

		// is this different from the current value?
		if(value != app.GetGameSettings(m_iPad,eGameSetting_UISize)+1)
		{
			app.SetGameSettings(m_iPad,eGameSetting_UISize,value-1);
			// Apply the changes to the selected text position
			ui.UpdateSelectedItemPos(m_iPad);
		}

		break;
	case eControl_UISizeSplitscreen:
		m_sliderUISizeSplitscreen.handleSliderMove(value);

		swprintf( (WCHAR *)TempString, 256, L"%ls: %d", app.GetString( IDS_SLIDER_UISIZESPLITSCREEN ),value);			
		m_sliderUISizeSplitscreen.setLabel(TempString);

		if(value != app.GetGameSettings(m_iPad,eGameSetting_UISizeSplitscreen)+1)
		{
			// slider is 1 to 3
			app.SetGameSettings(m_iPad,eGameSetting_UISizeSplitscreen,value-1);
			// Apply the changes to the selected text position
			ui.UpdateSelectedItemPos(m_iPad);
		}

		break;
	}
}
