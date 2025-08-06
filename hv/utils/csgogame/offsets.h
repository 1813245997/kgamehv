#pragma once
 
namespace updater {
	 
		// Global offsets
		inline constexpr int build_number = 14092;
		inline constexpr int dwBuildNumber = 6001860;
		inline constexpr int dwEntityList = 30139936;
		inline constexpr int dwLocalPlayer = 28265344;
		inline constexpr int dwPlantedC4 = 30568832;
		inline constexpr int dwViewMatrix = 30546432;
		inline constexpr int dwLocalPlayerController = 30474816;

		// Netvars
		inline constexpr int m_ArmorValue = 10468;
		inline constexpr int m_bIsDefusing = 10418;
		inline constexpr int m_flC4Blow = 4512;
		inline constexpr int m_flFlashOverlayAlpha = 5744;
		inline constexpr int m_flNextBeep = 4508;
		inline constexpr int m_flTimerLength = 4520;
		inline constexpr int m_hPlayerPawn = 2300;
		inline constexpr int m_iAccount = 64;
		inline constexpr int m_iHealth = 844;
		inline constexpr int m_iTeamNum = 1003;
		inline constexpr int m_pClippingWeapon = 5664;
		inline constexpr int m_pGameSceneNode = 816;
		inline constexpr int m_pInGameMoneyServices = 2040;
		inline constexpr int m_sSanitizedPlayerName = 2128;
		inline constexpr int m_szName = 1816;
		inline constexpr int m_vOldOrigin = 5552;
		inline constexpr int m_vecAbsOrigin = 208;
		inline constexpr int m_hController = 5576;
		inline constexpr int m_iszPlayerName = 1768;

		constexpr   unsigned long long m_hook_offset = 0X84BE42; //client.dll+7D7CCD;
 
	 
}
