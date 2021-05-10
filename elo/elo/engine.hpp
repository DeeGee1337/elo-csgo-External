#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <TlHelp32.h>
#include <optional>

#include "matrix.hpp"
#include "vector.hpp"
#include "offsets.hpp"

constexpr int max_player_index = 64;


bool write_memory(uintptr_t address, void* buffer, size_t data_length);
bool read_memory(uintptr_t address, void* buffer, size_t data_length);

template<class t>
t read(uintptr_t address)
{
	t val;
	read_memory(address, &val, sizeof(t));
	return val;
}

template<class t>
void write(uintptr_t address, const t& val)
{
	write_memory(address,(void*) &val, sizeof(t));
		/*if (!write_memory(address, &val, sizeof(t)))
			throw std::runtime_error("failed to write to memory");*/

}


#include <pshpack1.h>
struct glow_object
{
	uintptr_t entity;
	float r;
	float g;
	float b;
	float a;
	char junk01[16];
	bool render_when_occluded;
	bool render_when_unoccluded;
	bool full_bloom;
	char junk02[17];
};
static_assert(sizeof(glow_object) == 0x38, "Invalid user_cmd size");

struct user_cmd
{
	BYTE u1[4];//vtable
	int command_number;
	int tick_count;
	vec3 viewangles;
	vec3 aimdirection;
	float forwardmove;
	float sidemove;
	float upmove;
	int buttons;
	BYTE impulse;
	int weaponselect;
	int weaponsubtype;
	int random_seed;
	short mousedx;
	short mousedy;
	bool hasbeenpredicted;
	vec3 headangles;
	vec3 headoffset;
	short unknown1;
	int unknown2;
};
static_assert(sizeof(user_cmd) == 0x64, "Invalid user_cmd size");

struct studiohdr_t
{
public:
	__int32 id; //0x0000 
	__int32 version; //0x0004 
	__int32 checksum; //0x0008 
	char name[64]; //0x6F5888 
	__int32 len; //0x004C 
	vec3 vecEyePos; //0x0050 
	vec3 vecIllumPos; //0x005C 
	vec3 vecHullMin; //0x0068 
	vec3 vecHullMax; //0x0074 
	vec3 vecBBMin; //0x0080 
	vec3 vecBBMax; //0x008C 
	__int32 flags; //0x0098 
	__int32 numbones; //0x009C 
	__int32 boneindex; //0x00A0 
	__int32 numbonecontrollers; //0x00A4 
	__int32 bonecontrollerindex; //0x00A8 
	__int32 numhitboxsets; //0x00AC 
	__int32 hitboxsetindex; //0x00B0 
	__int32 N00000055; //0x00B4 
	__int32 N00000056; //0x00B8 
	__int32 N00000057; //0x00BC 
	__int32 N00000058; //0x00C0 
	__int32 N00000059; //0x00C4 
	__int32 N0000005A; //0x00C8 
	__int32 N0000005B; //0x00CC 
	DWORD N0000005C; //0x00D0 
	char pad_0x00D4[0x330]; //0x00D4
}; //Size=0x0404
struct msstudiobone_t
{
public:
	__int32 nameindex; //0x0000 
	__int32 parent; //0x0004 
	__int32 N0000016F; //0x0008 
	__int32 N00000170; //0x000C 
	__int32 N00000171; //0x0010 
	__int32 N00000172; //0x0014 
	__int32 N00000173; //0x0018 
	__int32 N00000174; //0x001C 
	vec3 pos; //0x0020 
	float quat[4]; //0x002C 
	vec3 rot; //0x003C 
	vec3 posscale; //0x0048 
	vec3 rotscale; //0x0054 
	matrix3x4 poseToBone; //0x0060 
	float qAlignment[4]; //0x0090 
	__int32 flags; //0x00A0 
	__int32 N00000180; //0x00A4 
	__int32 N000001F1; //0x00A8 
	__int32 N000001F9; //0x00AC 
	__int32 surfacepropidx; //0x00B0 
	__int32 contents; //0x00B4 
	__int32 surfacepropLookup; //0x00B8 
	char pad_0x00BC[0x1C]; //0x00BC

}; //Size=0x00D8

static_assert(sizeof(msstudiobone_t) == 0xD8, "invalid msstudiobone size");

struct player_info_t
{
private:
	DWORD __pad0[2];
public:
	int m_nXuidLow;
	int m_nXuidHigh;
	char m_szPlayerName[128];
	int m_nUserID;
	char m_szSteamID[33];
	UINT m_nSteam3ID;
	char m_szFriendsName[128];
	bool m_bIsFakePlayer;
	bool m_bIsHLTV;
	DWORD m_dwCustomFiles[4];
	BYTE m_FilesDownloaded;
private:
	int __pad1;
};

#include <poppack.h>

struct entitylist_t
{
	struct entry_t
	{
		uintptr_t entity_ptr;
		int flags;
		uintptr_t unknown1;
		uintptr_t unknown2;
	};

	static_assert(sizeof(entry_t) == 16, "invalid entry_t size");

	entry_t* entities;
	int size;

	entitylist_t(uintptr_t entity_list_ptr, int max_count)
	{
		size = max_count;
		entities = (entry_t*)malloc(sizeof(entry_t) * max_count);
		if (!entities)
			throw std::runtime_error("failed to allocate entity_list");
			
		memset(entities, 0, sizeof(entry_t) * max_count);
		read_memory(entity_list_ptr + 16 * (0 - 0x2001), entities, sizeof(entry_t) * max_count);
	}
	~entitylist_t()
	{
		free(entities);
	}
	uintptr_t get(int index)
	{
		return entities[index].entity_ptr;
	}
};

struct player_t
{
	uintptr_t ptr;
	int index;
	int team;
	int health;
	vec3 head;
	player_info_t info;
	int weapon_id;

	struct playerbone_t
	{
		int index;
		uintptr_t ptr;
		msstudiobone_t data;
	};

	std::vector<playerbone_t> bones;
};

struct skeleton_t
{
	struct bone_t
	{
		int index;
		uintptr_t ptr;
		msstudiobone_t data;
		vec3 position;
		bool is_visible;
	};

	uintptr_t ptr;
	int player_index;
	std::vector<bone_t> bones;
	bool is_visible;
};

using player_ptr = std::shared_ptr<player_t>;
using skeleton_ptr = std::shared_ptr<skeleton_t>;

enum weapon_category : uint16_t
{
	wpn_category_none,
	wpn_category_pistol,
	wpn_category_rifle,
	wpn_category_sniper,
	wpn_category_smg,
	wpn_category_shotgun,
	wpn_category_mg,
	wpn_category_nade,
	wpn_category_other
};

enum weapon_id : uint16_t
{
	weapon_none,
	weapon_deagle,
	weapon_elite,
	weapon_fiveseven,
	weapon_glock,
	weapon_p228,
	weapon_usp,
	weapon_ak47,
	weapon_aug,
	weapon_awp,
	weapon_famas,
	weapon_g3sg1,
	weapon_galil,
	weapon_galilar,
	weapon_m249,
	weapon_m3,
	weapon_m4a1,
	weapon_mac10,
	weapon_mp5navy,
	weapon_p90,
	weapon_scout,
	weapon_sg550,
	weapon_sg552,
	weapon_tmp,
	weapon_ump45,
	weapon_xm1014,
	weapon_bizon,
	weapon_mag7,
	weapon_negev,
	weapon_sawedoff,
	weapon_tec9,
	weapon_taser,
	weapon_hkp2000,
	weapon_mp7,
	weapon_mp9,
	weapon_nova,
	weapon_p250,
	weapon_scar17,
	weapon_scar20,
	weapon_sg556,
	weapon_ssg08,
	weapon_knifegg,
	weapon_knife,
	weapon_flashbang,
	weapon_hegrenade,
	weapon_smokegrenade,
	weapon_molotov,
	weapon_decoy,
	weapon_incgrenade,
	weapon_c4,
	weapon_knife_t = 59,
	weapon_m4a1_silencer = 60,
	weapon_usp_silencer = 61,
	weapon_cz75a = 63,
	weapon_revolver = 64,
	weapon_knife_bayonet = 500,
	weapon_knife_flip = 505,
	weapon_knife_gut = 506,
	weapon_knife_karambit = 507,
	weapon_knife_m9_bayonet = 508,
	weapon_knife_tactical = 509,
	weapon_knife_falchion = 512,
	weapon_knife_survival_bowie = 514,
	weapon_knife_butterfly = 515,
	weapon_knife_push = 516,
	glove_studded_bloodhound = 5027,
	glove_t_side = 5028,
	glove_ct_side = 5029,
	glove_sporty = 5030,
	glove_slick = 5031,
	glove_leather_wrap = 5032,
	glove_motorcycle = 5033,
	glove_specialist = 5034,
};


std::optional<DWORD>	get_process_id(std::wstring name);
uintptr_t				get_module_base(std::wstring name);
uintptr_t				read_pointer(uintptr_t address);
std::string				read_string_ptr(uintptr_t address, size_t len);

bool					is_ingame();
uintptr_t				get_entity_list();
int						get_health(uintptr_t entity);
int						get_armor(uintptr_t entity);
bool					is_dormant(uintptr_t entity);
uintptr_t				get_entity(uintptr_t entity_list, int index);
int						get_team(uintptr_t entity);
bool					has_sound(uintptr_t entity);


int						get_entity_index(uintptr_t entity);
uintptr_t				get_glow_object_ptr(uintptr_t glow_manager, int index);
uintptr_t				get_user_cmd_ptr(int command_number);
uintptr_t				get_client_state_ptr();
vec3					get_origin(uintptr_t entity);
int						get_class_id(uintptr_t entity);
vec3					get_bone_position(uintptr_t entity, int bone_id);
bool					world_to_screen(const vec3& vecOrigin, vec3& vecScreen);

int						get_weapon_type(int weapon_id);
std::string				get_weapon_name(int weapon_id);
bool					is_non_aim_weapon(int weapon_id);

player_info_t			get_player_info(int index);
uintptr_t				get_studio_hdr(uintptr_t entity, studiohdr_t& hdr);
uintptr_t				get_studio_bone(uintptr_t hdr_ptr, studiohdr_t& hdr, int index, msstudiobone_t& bone);
std::string				get_bone_name(uintptr_t bone_ptr, msstudiobone_t& bone);

std::string				get_game_path();
std::string				get_map_path();	
vec3					get_view_position();
bool					is_visible(uintptr_t entity, vec3 position);
bool					is_visible_trace(uintptr_t entity, vec3 position);
