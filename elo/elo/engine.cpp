#include "engine.hpp"
#include "offsets.hpp"
#include "csgo.hpp"

uintptr_t read_pointer(uintptr_t address)
{
	if (address == 0)
		return 0;

	return read<uintptr_t>(address);
}

std::string read_string_ptr(uintptr_t address, size_t len)
{
	std::string str;
	str.resize(len);

	read_memory(read_pointer(address), str.data(), len);
	str.resize(strlen(str.c_str()));

	return str;
}

std::string read_string(uintptr_t address, size_t len)
{
	std::string str;
	str.resize(len);

	read_memory(address, str.data(), len);
	str.resize(strlen(str.c_str()));
	return str;
}

bool read_memory(uintptr_t address, void* buffer, size_t data_length)
{
	SIZE_T bytes_read = 0;
	return ReadProcessMemory(g_csgo.process_handle, (void*)address, buffer, data_length, &bytes_read);
}

bool write_memory(uintptr_t address, void* buffer, size_t data_length)
{
	SIZE_T bytes_written = 0;
	return WriteProcessMemory(g_csgo.process_handle, (void*)address, buffer, data_length, &bytes_written) != FALSE;
}

bool is_dormant(uintptr_t entity)
{
	return read<bool>(entity + hazedumper::signatures::m_bDormant);
}

int get_health(uintptr_t entity)
{
	return read<int>(entity + hazedumper::netvars::m_iHealth);
}

int get_armor(uintptr_t entity)
{
	return read<int>(entity + hazedumper::netvars::m_ArmorValue);
}

bool is_ingame()
{
	auto client_state = read_pointer(g_csgo.engine + hazedumper::signatures::dwClientState);
	if (!client_state)
		return false;

	auto signon_state = read<int>(client_state + hazedumper::signatures::dwClientState_State);
	if (signon_state != 6)
		return false;

	return true;
}

uintptr_t get_entity_list()
{
	return read_pointer(g_csgo.engine + hazedumper::signatures::ptrEntityList);
}

uintptr_t get_entity(uintptr_t entity_list, int index)
{
	return read_pointer(entity_list + 16 * (index - 0x2001));
}

int	get_team(uintptr_t entity)
{
	return read<int>(entity + hazedumper::netvars::m_iTeamNum);
}

bool has_sound(uintptr_t entity)
{
	auto addr = g_csgo.engine + 0x63DD90;

	auto entid = read<DWORD>(addr + 0xF4);
	return get_entity_index(entity) == entid;
}

int get_entity_index(uintptr_t entity)
{
	return read<int>(g_csgo.local_player + 0x64) - 1;
}

bool is_visible(uintptr_t entity, vec3 position)
{
	int mask = read<int>(entity + hazedumper::netvars::m_bSpottedByMask);
	int index = read<int>(g_csgo.local_player + 0x64) - 1;

	if (read<bool>(entity + hazedumper::netvars::m_bSpotted)
		&& (mask & (1 << index)) > 0)
		return true;
	return false;
}

bool is_visible_trace(uintptr_t entity, vec3 position)
{
	auto skel = g_csgo.skeleton(entity);
	if (!skel)
		return false;

	return skel->is_visible;
}

vec3 get_view_position()
{
	auto origin = get_origin(g_csgo.local_player);
	auto view_offset = read<vec3>(g_csgo.local_player + hazedumper::netvars::m_vecViewOffset);

	return origin + view_offset;
}

uintptr_t get_glow_object_ptr(uintptr_t glow_manager, int index)
{
	return glow_manager + (sizeof(glow_object) * index);
}

uintptr_t get_user_cmd_ptr(int command_number)
{
	auto input = g_csgo.client + hazedumper::signatures::dwInput;
	if (!input)
		return 0;

	auto user_cmd_arr = read_pointer(input + 0xF4);
	if (!user_cmd_arr)
		return 0;

	return user_cmd_arr + (sizeof(user_cmd) * (command_number % 150));
}

vec3 get_origin(uintptr_t entity)
{
	return read<vec3>(entity + hazedumper::netvars::m_vecOrigin);
}

uintptr_t get_client_state_ptr()
{
	return read_pointer(g_csgo.engine + hazedumper::signatures::dwClientState);
}

int get_class_id(uintptr_t entity)
{
	auto networkable = read_pointer(entity + 0x8);
	auto client_class_fn = read_pointer(networkable + 0x8);
	auto class_ptr = read_pointer(client_class_fn + 1);

	return read<int>(class_ptr + 0x14);
}

uintptr_t get_studio_hdr(uintptr_t entity, studiohdr_t& hdr)
{
	auto ptr = read_pointer(entity + hazedumper::signatures::m_pStudioHdr);
	if (!ptr)
		return 0;

	auto studio_ptr = read_pointer(ptr);
	if (!studio_ptr)
		return 0;

	hdr = read<studiohdr_t>(studio_ptr);

	return studio_ptr;
}

uintptr_t get_studio_bone(uintptr_t hdr_ptr, studiohdr_t& hdr, int index, msstudiobone_t& bone)
{
	if (index > hdr.numbones)
		return 0;

	auto ptr = hdr_ptr + hdr.boneindex + (sizeof(msstudiobone_t) * index);
	
	bone = read<msstudiobone_t>(ptr);

	return ptr;
}

std::string get_bone_name(uintptr_t bone_ptr, msstudiobone_t& bone)
{
	auto ptr = bone_ptr + bone.nameindex;

	return read_string(ptr, 50);
}

std::string get_map_path()
{
	if (!g_csgo.client_state)
		return {};

	return read_string(g_csgo.client_state + hazedumper::signatures::dwClientState_MapDirectory, 255);
}

std::string get_game_path()
{
	if (!g_csgo.engine)
		return {};

	return read_string(g_csgo.engine + hazedumper::signatures::dwGameDir, 255);
}

player_info_t get_player_info(int index)
{
	if (!g_csgo.user_info)
		return {};

	auto player_info_ptr = read_pointer(g_csgo.user_info + 0x28 + (0x34 * (index - 1)));
	if (!player_info_ptr)
		return {};

	return read<player_info_t>(player_info_ptr);
}

vec3 get_bone_position(uintptr_t entity, int bone_id)
{
	auto bone_array_ptr = read_pointer(entity + hazedumper::netvars::m_dwBoneMatrix);
	if (!bone_array_ptr)
		return {};

	auto bone_matrix = read<matrix3x4>(bone_array_ptr + (0x30 * bone_id));
	return { bone_matrix[0][3], bone_matrix[1][3], bone_matrix[2][3] };
}

bool world_to_screen(const vec3& vecOrigin, vec3& vecScreen)
{
	vecScreen.x = g_csgo.view_matrix[0] * vecOrigin.x + g_csgo.view_matrix[1] * vecOrigin.y + g_csgo.view_matrix[2] * vecOrigin.z + g_csgo.view_matrix[3];
	vecScreen.y = g_csgo.view_matrix[4] * vecOrigin.x + g_csgo.view_matrix[5] * vecOrigin.y + g_csgo.view_matrix[6] * vecOrigin.z + g_csgo.view_matrix[7];

	auto flTemp = g_csgo.view_matrix[12] * vecOrigin.x + g_csgo.view_matrix[13] * vecOrigin.y + g_csgo.view_matrix[14] * vecOrigin.z + g_csgo.view_matrix[15];

	if (flTemp < 0.01f)
		return false;

	auto invFlTemp = 1.f / flTemp;
	vecScreen.x *= invFlTemp;
	vecScreen.y *= invFlTemp;

	static int iResolution[2] = { 0 };
	if (!iResolution[0] || !iResolution[1])
	{
		iResolution[0] = g_csgo.surface->get_width();
		iResolution[1] = g_csgo.surface->get_height();
	}

	auto x = (float)iResolution[0] / 2.f;
	auto y = (float)iResolution[1] / 2.f;

	x += 0.5f * vecScreen[0] * (float)iResolution[0] + 0.5f;
	y -= 0.5f * vecScreen[1] * (float)iResolution[1] + 0.5f;

	vecScreen.x = x;
	vecScreen.y = y;

	return true;
}

bool is_non_aim_weapon(int weapon_id)
{
	if (weapon_id == weapon_knifegg || weapon_id == weapon_knife || weapon_id == weapon_flashbang || weapon_id == weapon_hegrenade || weapon_id == weapon_smokegrenade
		|| weapon_id == weapon_molotov || weapon_id == weapon_decoy || weapon_id == weapon_incgrenade || weapon_id == weapon_c4)
		return true;

	return false;
}

int get_weapon_type(int weapon_id)
{
	int type = wpn_category_none;
	if (weapon_id == weapon_deagle || weapon_id == weapon_elite || weapon_id == weapon_fiveseven || weapon_id == weapon_glock || weapon_id == weapon_tec9 || weapon_id == weapon_usp_silencer || weapon_id == weapon_p250 || weapon_id == weapon_usp_silencer || weapon_id == weapon_cz75a || weapon_id == weapon_revolver)
		type = wpn_category_pistol;
	if (weapon_id == weapon_knife || weapon_id == weapon_c4 || weapon_id == weapon_knife_t || weapon_id == weapon_knife_bayonet || weapon_id == weapon_knife_flip || weapon_id == weapon_knife_gut || weapon_id == weapon_knife_karambit || weapon_id == weapon_knife_m9_bayonet || weapon_id == weapon_knife_tactical || weapon_id == weapon_knife_falchion || weapon_id == weapon_knife_survival_bowie || weapon_id == weapon_knife_butterfly || weapon_id == weapon_knife_push)
		type = wpn_category_other;
	if (weapon_id == weapon_flashbang || weapon_id == weapon_hegrenade || weapon_id == weapon_smokegrenade || weapon_id == weapon_molotov || weapon_id == weapon_decoy || weapon_id == weapon_incgrenade)
		type = wpn_category_nade;
	if (weapon_id == weapon_ak47 || weapon_id == weapon_aug || weapon_id == weapon_famas || weapon_id == weapon_galilar || weapon_id == weapon_m4a1 || weapon_id == weapon_sg556 || weapon_id == weapon_m4a1_silencer)
		type = wpn_category_rifle;
	if (weapon_id == weapon_awp || weapon_id == weapon_g3sg1 || weapon_id == weapon_scar20 || weapon_id == weapon_ssg08)
		type = wpn_category_sniper;
	if (weapon_id == weapon_mac10 || weapon_id == weapon_p90 || weapon_id == weapon_ump45 || weapon_id == weapon_bizon || weapon_id == weapon_mp7 || weapon_id == weapon_mp9)
		type = wpn_category_smg;
	if (weapon_id == weapon_xm1014 || weapon_id == weapon_mag7 || weapon_id == weapon_sawedoff || weapon_id == weapon_nova)
		type = wpn_category_shotgun;
	if (weapon_id == weapon_m249 || weapon_id == weapon_negev)
		type = wpn_category_mg;

	return type;
}

std::string get_weapon_name(int id) 
{
	switch (id)
	{
	case weapon_deagle:
		return ("deagle");
	case weapon_elite:
		return ("dual berettas");
	case weapon_fiveseven:
		return ("five-seven");
	case weapon_glock:
		return ("glock-18");
	case weapon_ak47:
		return ("ak-47");
	case weapon_aug:
		return ("aug");
	case weapon_awp:
		return ("awp");
	case weapon_famas:
		return ("famas");
	case weapon_g3sg1:
		return ("g3sg1");
	case weapon_galilar:
		return ("galil");
	case weapon_m249:
		return ("m249");
	case weapon_m4a1:
		return ("m4a1");
	case weapon_mac10:
		return ("mac-10");
	case weapon_p90:
		return ("p90");
	case weapon_ump45:
		return ("ump-45");
	case weapon_xm1014:
		return ("xm1014");
	case weapon_bizon:
		return ("pp-bizon");
	case weapon_mag7:
		return ("mag-7");
	case weapon_negev:
		return ("negev");
	case weapon_sawedoff:
		return ("sawed-off");
	case weapon_tec9:
		return ("tec-9");
	case weapon_taser:
		return ("taser");
	case weapon_hkp2000:
		return ("p2000");
	case weapon_mp7:
		return ("mp7");
	case weapon_mp9:
		return ("mp9");
	case weapon_nova:
		return ("nova");
	case weapon_p250:
		return ("p250");
	case weapon_scar20:
		return ("scar-20");
	case weapon_sg556:
		return ("sg 553");
	case weapon_ssg08:
		return ("ssg 08");
	case weapon_knife:
		return ("knife");
	case weapon_flashbang:
		return ("flashbang");
	case weapon_hegrenade:
		return ("he grenade");
	case weapon_smokegrenade:
		return ("smoke grenade");
	case weapon_molotov:
		return ("molly");
	case weapon_decoy:
		return ("decoy");
	case weapon_incgrenade:
		return ("incendiary grenade");
	case weapon_c4:
		return ("c4");
	case weapon_m4a1_silencer:
		return ("m4a1-s");
	case weapon_usp_silencer:
		return ("usp-s");
	case weapon_cz75a:
		return ("cz75-auto");
	default:
		return ("knife");
	}
	return "unknown";
}


std::optional<DWORD> get_process_id(std::wstring name)
{
	auto handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (handle == INVALID_HANDLE_VALUE)
		return {};

	PROCESSENTRY32 proc_entry{};
	proc_entry.dwSize = sizeof(proc_entry);

	if (Process32First(handle, &proc_entry))
	{
		do
		{
			if (_wcsicmp(proc_entry.szExeFile, name.c_str()) == 0)
			{
				CloseHandle(handle);
				return { proc_entry.th32ProcessID };
			}

		} while (Process32Next(handle, &proc_entry));
	}

	CloseHandle(handle);

	return {};
}

uintptr_t get_module_base(std::wstring name)
{
	auto handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, g_csgo.process_id);
	if (handle == INVALID_HANDLE_VALUE)
		return {};

	MODULEENTRY32 module_entry{};
	module_entry.dwSize = sizeof(module_entry);

	if (Module32First(handle, &module_entry))
	{
		do
		{
			if (_wcsicmp(module_entry.szModule, name.c_str()) == 0)
			{
				CloseHandle(handle);
				return (uintptr_t)module_entry.modBaseAddr;
			}

		} while (Module32Next(handle, &module_entry));
	}

	CloseHandle(handle);

	return 0;
}