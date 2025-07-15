#pragma once
 
namespace cs2SDK {
	namespace offsets
	{
		// 游戏版本号的偏移量
		constexpr 	unsigned long long dwBuildNumber = 0x532BE4;

		// 玩家场景节点指针的偏移量，用于处理玩家的3D位置信息
		constexpr 	unsigned long long m_pGameSceneNode = 0x328; // CGameSceneNode*

		// 本地玩家控制器的偏移量
		constexpr 	unsigned long long dwLocalPlayerController = 0x1A52D20;


		// 视图矩阵的偏移量，用于绘制3D到2D坐标
		constexpr 	unsigned long long dwViewMatrix = 0x1A6D280;

		// 放置C4的偏移量， 
		constexpr 	unsigned long long  dwPlantedC4 = 0x1A71C40;

		// C4爆炸时间的偏移量
		constexpr 	unsigned long long m_flC4Blow = 0xFC0; // GameTime_t

		// C4下次嘀嗒声时间的偏移量
		constexpr 	unsigned long long m_flNextBeep = 0xFBC; // GameTime_t

		// C4计时长度的偏移量
		constexpr 	unsigned long long m_flTimerLength = 0xFC8; // float32

		// 玩家账户金钱数的偏移量
		constexpr 	unsigned long long m_iAccount = 0x40; // int32

		// 玩家绝对位置的偏移量
		constexpr 	unsigned long long m_vecAbsOrigin = 0xD0; // Vector

		// 玩家旧位置的偏移量
		constexpr 	unsigned long long m_vOldOrigin = 0x1324;  // Vector

		// 玩家是否正在拆弹的偏移量
		constexpr 	unsigned long long m_bIsDefusing = 0x23EA; // bool

		// 玩家名字的偏移量
		constexpr 	unsigned long long m_szName = 0xD20; // CGlobalSymbol

		// 当前持有武器的偏移量
		constexpr 	unsigned long long m_pClippingWeapon = 0x13A0; // C_CSWeaponBase*

		// 玩家护甲值的偏移量
		constexpr 	unsigned long long m_ArmorValue = 0x241C;  // int32

		// 玩家血量的偏移量
		constexpr 	unsigned long long m_iHealth = 0x344; // int32

		// 玩家是否活着
		constexpr 	unsigned long long m_bPawnIsAlive = 0x82C; // bool


		// 玩家对象指针的偏移量
		constexpr 	unsigned long long m_hPlayerPawn = 0x824; // CHandle<C_CSPlayerPawn>

		// 玩家名字（已处理敏感内容）的偏移量
		constexpr 	unsigned long long m_sSanitizedPlayerName = 0x778; // CUtlString

		//这个也是玩家位置
		constexpr 	unsigned long long m_vLastSlopeCheckPos = 0xDB8; // Vector

		// 玩家队伍编号的偏移量，表示玩家所属的队伍（如CT、T阵营）
		constexpr 	unsigned long long m_iTeamNum = 0x3E3; // uint8

		// 游戏内金钱服务指针的偏移量，指向玩家的金钱服务对象
		constexpr 	unsigned long long m_pInGameMoneyServices = 0x720; // CCSPlayerController_InGameMoneyServices*

		// 玩家屏幕闪光效果透明度的偏移量，表示闪光弹效果的强度
		constexpr 	unsigned long long m_flFlashOverlayAlpha = 0x1400; // float32


		//下面这几个是自己根据结构分析的
		//本地玩家指针
		constexpr 	unsigned long long dwLocalPlayerPawn = 0X18580D0; //DUMP的不准

		// 玩家队伍偏移量
		constexpr 	unsigned long long m_offsettema = 0xE68;

		// 玩家数组偏移
		constexpr 	unsigned long long m_offestPlayerArray = 0X18652B0;//

		// 玩家是否存在的偏移量
		constexpr 	unsigned long long m_offestbIsPlayerExists = 0x320; // 新增

		constexpr 	unsigned long long m_OoffsetBone = 0x1F0; // CGameSceneNode*
	}
}
