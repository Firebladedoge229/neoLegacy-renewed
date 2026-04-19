#include "stdafx.h"
#include "UI.h"
#include "UIScene_HelpAndOptionsMenu.h"
#include "../../Minecraft.h"

void UIScene_HelpAndOptionsMenu::cacheBaseButtonPositions()
{
	if(m_hasCachedBaseButtonY)
	{
		return;
	}

	m_buttons[BUTTON_HAO_SETTINGS].UpdateControl();
	m_buttons[BUTTON_HAO_CREDITS].UpdateControl();
	m_buttons[BUTTON_HAO_REINSTALL].UpdateControl();
	m_buttons[BUTTON_HAO_DEBUG].UpdateControl();

	m_baseSettingsY = m_buttons[BUTTON_HAO_SETTINGS].getYPos();
	m_baseCreditsY = m_buttons[BUTTON_HAO_CREDITS].getYPos();
	m_baseReinstallY = m_buttons[BUTTON_HAO_REINSTALL].getYPos();
	m_baseDebugY = m_buttons[BUTTON_HAO_DEBUG].getYPos();
	m_hasCachedBaseButtonY = true;
}

void UIScene_HelpAndOptionsMenu::setButtonY(int buttonIndex, int yPos)
{
	if(buttonIndex < 0 || buttonIndex >= BUTTONS_HAO_MAX)
	{
		return;
	}

	IggyName nameY = registerFastName(L"y");
	IggyValueSetF64RS(m_buttons[buttonIndex].getIggyValuePath(), nameY, nullptr, static_cast<F64>(yPos));
}

void UIScene_HelpAndOptionsMenu::refreshDeveloperSettingsButtonState()
{
	cacheBaseButtonPositions();

	bool showDeveloperSettings = false;
#ifndef _FINAL_BUILD
	const bool isPrimaryPad = (ProfileManager.GetPrimaryPad() == m_iPad);
	const bool allowDeveloperSettings = (app.GetLocalPlayerCount() <= 1) || isPrimaryPad;
	showDeveloperSettings = app.DebugSettingsOn() && allowDeveloperSettings;
#endif
	m_buttons[BUTTON_HAO_DEBUG].setEnable(showDeveloperSettings);
	m_buttons[BUTTON_HAO_DEBUG].setVisible(showDeveloperSettings);

	if(!m_hasCachedBaseButtonY)
	{
		return;
	}

	const int rowSpacing = (m_baseCreditsY > m_baseSettingsY) ? (m_baseCreditsY - m_baseSettingsY) : 36;

	if(showDeveloperSettings && !m_buttons[BUTTON_HAO_CREDITS].getHidden())
	{
		setButtonY(BUTTON_HAO_DEBUG, m_baseCreditsY);
		setButtonY(BUTTON_HAO_CREDITS, m_baseCreditsY + rowSpacing);

		if(!m_buttons[BUTTON_HAO_REINSTALL].getHidden())
		{
			setButtonY(BUTTON_HAO_REINSTALL, m_baseReinstallY + rowSpacing);
		}
	}
	else
	{
		setButtonY(BUTTON_HAO_DEBUG, m_baseDebugY);
		setButtonY(BUTTON_HAO_CREDITS, m_baseCreditsY);

		if(!m_buttons[BUTTON_HAO_REINSTALL].getHidden())
		{
			setButtonY(BUTTON_HAO_REINSTALL, m_baseReinstallY);
		}
	}
}

UIScene_HelpAndOptionsMenu::UIScene_HelpAndOptionsMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	m_hasCachedBaseButtonY = false;
	m_baseSettingsY = 0;
	m_baseCreditsY = 0;
	m_baseReinstallY = 0;
	m_baseDebugY = 0;

	// Setup all the Iggy references we need for this scene
	initialiseMovie();

	m_bNotInGame=(Minecraft::GetInstance()->level==nullptr);

	m_buttons[BUTTON_HAO_CHANGESKIN].init(IDS_CHANGE_SKIN,BUTTON_HAO_CHANGESKIN);
	m_buttons[BUTTON_HAO_HOWTOPLAY].init(IDS_HOW_TO_PLAY,BUTTON_HAO_HOWTOPLAY);
	m_buttons[BUTTON_HAO_CONTROLS].init(IDS_CONTROLS,BUTTON_HAO_CONTROLS);
	m_buttons[BUTTON_HAO_SETTINGS].init(IDS_SETTINGS,BUTTON_HAO_SETTINGS);
	m_buttons[BUTTON_HAO_CREDITS].init(IDS_CREDITS,BUTTON_HAO_CREDITS);
	//m_buttons[BUTTON_HAO_REINSTALL].init(app.GetString(IDS_REINSTALL_CONTENT),BUTTON_HAO_REINSTALL);
	m_buttons[BUTTON_HAO_DEBUG].init(L"Developer Settings",BUTTON_HAO_DEBUG);
	cacheBaseButtonPositions();

	/* 4J-TomK - we should never remove a control before the other buttons controls are initialised!
	(because vita touchboxes are rebuilt on remove since the remaining positions might change) */
	// We don't have a reinstall content, so remove the button
	removeControl( &m_buttons[BUTTON_HAO_REINSTALL], false );

#ifdef _XBOX_ONE
	// 4J-PB - in order to buy the skin packs, we need the signed offer ids for them, which we get in the availability info
	// we need to retrieve this info though, so do it here
	app.AddDLCRequest(e_Marketplace_Content); // content is skin packs, texture packs and mash-up packs

	// we also need to mount the local DLC so we can tell what's been purchased
	app.StartInstallDLCProcess(iPad);
#endif



	// 4J-PB - do not need a storage device to see this menu - just need one when you choose to re-install them
	bool bNotInGame=(Minecraft::GetInstance()->level==nullptr);

	// any content to be re-installed? 
	if(m_iPad==ProfileManager.GetPrimaryPad() && bNotInGame)
	{
		// We should show the reinstall menu
		app.DebugPrintf("Reinstall Menu required...\n");
	}
	else
	{
		removeControl( &m_buttons[BUTTON_HAO_REINSTALL], false);
	}

	if(app.GetLocalPlayerCount()>1)
	{
		// no credits in splitscreen
		removeControl( &m_buttons[BUTTON_HAO_CREDITS], false);

#if TO_BE_IMPLEMENTED
		app.AdjustSplitscreenScene(m_hObj,&m_OriginalPosition,m_iPad,false);
#endif
		if(ProfileManager.GetPrimaryPad()!=m_iPad)
		{
			removeControl( &m_buttons[BUTTON_HAO_REINSTALL], false);
		}
	}

	if(!ProfileManager.IsFullVersion() )//|| ProfileManager.IsGuest(m_iPad))
	{
		removeControl( &m_buttons[BUTTON_HAO_CHANGESKIN], false);
	}

	refreshDeveloperSettingsButtonState();

	// 4J-TomK Moved horizontal resize check to the end to prevent horizontal scaling for buttons that might get removed anyways (debug options for example)
	doHorizontalResizeCheck();

	//StorageManager.TMSPP_GetUserQuotaInfo(C4JStorage::eGlobalStorage_TitleUser,iPad);
	//StorageManager.WebServiceRequestGetFriends(iPad);
}

UIScene_HelpAndOptionsMenu::~UIScene_HelpAndOptionsMenu()
{
}

wstring UIScene_HelpAndOptionsMenu::getMoviePath()
{
	if(app.GetLocalPlayerCount() > 1)
	{
		return L"HelpAndOptionsMenuSplit";
	}
	else
	{
		return L"HelpAndOptionsMenu";
	}
}

void UIScene_HelpAndOptionsMenu::updateTooltips()
{
	ui.SetTooltips( m_iPad, IDS_TOOLTIPS_SELECT,IDS_TOOLTIPS_BACK);
}

void UIScene_HelpAndOptionsMenu::updateComponents()
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

void UIScene_HelpAndOptionsMenu::handleReload()
{
	refreshDeveloperSettingsButtonState();

	// 4J-PB - do not need a storage device to see this menu - just need one when you choose to re-install them
	bool bNotInGame=(Minecraft::GetInstance()->level==nullptr);

	// any content to be re-installed? 
	if(m_iPad==ProfileManager.GetPrimaryPad() && bNotInGame)
	{
		// We should show the reinstall menu
		app.DebugPrintf("Reinstall Menu required...\n");
	}
	else
	{
		removeControl( &m_buttons[BUTTON_HAO_REINSTALL], false);
	}

	if(app.GetLocalPlayerCount()>1)
	{
		// no credits in splitscreen
		removeControl( &m_buttons[BUTTON_HAO_CREDITS], false);

#if TO_BE_IMPLEMENTED
		app.AdjustSplitscreenScene(m_hObj,&m_OriginalPosition,m_iPad,false);
#endif
		if(ProfileManager.GetPrimaryPad()!=m_iPad)
		{
			removeControl( &m_buttons[BUTTON_HAO_REINSTALL], false);
		}
	}

	if(!ProfileManager.IsFullVersion() )//|| ProfileManager.IsGuest(m_iPad))
	{
#if TO_BE_IMPLEMENTED
		m_Buttons[BUTTON_HAO_CHANGESKIN].SetEnable(FALSE);
		m_Buttons[BUTTON_HAO_CHANGESKIN].EnableInput(FALSE);
		// set the focus to the second button

		XuiElementSetUserFocus(m_Buttons[BUTTON_HAO_HOWTOPLAY].m_hObj, m_iPad);
#endif
	}

	if(!ProfileManager.IsFullVersion() )//|| ProfileManager.IsGuest(m_iPad))
	{
		removeControl( &m_buttons[BUTTON_HAO_CHANGESKIN], false);
	}

	refreshDeveloperSettingsButtonState();

	doHorizontalResizeCheck();
}

void UIScene_HelpAndOptionsMenu::handleGainFocus(bool navBack)
{
	UIScene::handleGainFocus(navBack);
	refreshDeveloperSettingsButtonState();
}

void UIScene_HelpAndOptionsMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	//app.DebugPrintf("UIScene_DebugOverlay handling input for pad %d, key %d, down- %s, pressed- %s, released- %s\n", iPad, key, down?"TRUE":"FALSE", pressed?"TRUE":"FALSE", released?"TRUE":"FALSE");

	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed && !repeat)
		{
			navigateBack();
		}
		break;
	case ACTION_MENU_OK:
#ifdef __ORBIS__
	case ACTION_MENU_TOUCHPAD_PRESS:
#endif
	//CD - Added for audio
	if(pressed)
	{
		ui.PlayUISFX(eSFX_Press);
	}

	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
		sendInputToMovie(key, repeat, pressed, released);
		break;
	}
}

void UIScene_HelpAndOptionsMenu::handlePress(F64 controlId, F64 childId)
{
	switch(static_cast<int>(controlId))
	{
	case BUTTON_HAO_CHANGESKIN:
		ui.NavigateToScene(m_iPad, eUIScene_SkinSelectMenu);
		break;
	case BUTTON_HAO_HOWTOPLAY:
		ui.NavigateToScene(m_iPad, eUIScene_HowToPlayMenu);
		break;
	case BUTTON_HAO_CONTROLS:
		ui.NavigateToScene(m_iPad, eUIScene_ControlsMenu);
		break;
	case BUTTON_HAO_SETTINGS:
		ui.NavigateToScene(m_iPad, eUIScene_SettingsMenu);
		break;
	case BUTTON_HAO_CREDITS:
		ui.NavigateToScene(m_iPad, eUIScene_Credits);
		break;
	case BUTTON_HAO_REINSTALL:
		ui.NavigateToScene(m_iPad, eUIScene_ReinstallMenu);
		break;
	case BUTTON_HAO_DEBUG:
		ui.NavigateToScene(m_iPad, eUIScene_DebugOptions);
		break;
	}
}
