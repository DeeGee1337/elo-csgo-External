#pragma once
#include <cstdint>

struct Items
{
	int espFolder;

	int espGlow;
	int espPlayers;
	int espPlayersTeam;
	int espSkeleton;
	int espBox;
	int espBoxFilled;
	int espDistance;
	int espWeapon;
	int espHealth;
	int espArmor;
	int espName;
	int espMaxDistance;
	int espRadar2DDrawOutRanged;
	int espRadar2DCross;
	int espRadar2DBorder;
	int espRadar2DBackground;
	int espRadar2DSize;
	int espRadar2DMaxDistance;
	int espRadar2DScale;
	int espRadar2DPosition;
	int espRadar2D;
	int espGrenade;
	int espMapRadar;

	int espRadar;

	int aimbotFolder;
	int aimbotEnabled[5];
	int aimbotVisibleOnly[5];
	int aimbotKey[5];
	int aimbotBone[5];
	int aimbotCompensateRecoil[5];
	int aimbotSmooth[5];
	int aimbotFOV[5];
	int aimbotDistance[5];
	int aimbotNoTargetSwitch[5];
	int aimbotStayOnTarget[5];
	int rcsAimbot;
	int rcsStandalone;

	int triggerbot;
	int triggerbotKey;
	int bhop;

	int xray1r;
	int xray1g;
	int xray1b;
	int xray1a;

	int xray2r;
	int xray2g;
	int xray2b;
	int xray2a;
	int soundGlowOnly;
	int espLegit;

	int miscSettingsProfile;
	int miscSave;
	int miscLoad;


	int espLine;

	int espDrawHitmarker;
	int espHitmarkerSound;
	int espDrawCrosshair;

	int aimbotDrawFOV;

};

extern Items g_items;


void LoadItems(int profile);
void SaveItems(int profile);