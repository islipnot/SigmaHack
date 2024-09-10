#define MAX_FLOAT 18446744073709551615.0f

#include "pch.h"
#include "input.hpp"
#include "weapon.hpp"
#include "entities.hpp"
#include "pScanning.hpp"

UINT* player_count;
player_entity*** player_list_ptr;
player_entity* local_player;
extern menu_cfg cfg;

void aimbot()
{
	const auto player_list = *player_list_ptr;

	float closest_dst = MAX_FLOAT;
	float closest_aim = MAX_FLOAT;
	float most_danger = MAX_FLOAT;
	UINT lowest_hp = 1000;

	float final_yaw = -360;
	float final_pitch = -360;

	for (int x = 1; x < *player_count; ++x)
	{
		if (player_list[x]->dead || (player_list[x]->team == local_player->team && !cfg.target_team)) continue;

		float absX = local_player->x - player_list[x]->x;
		float absY = local_player->y - player_list[x]->y;
		float absZ = player_list[x]->z - local_player->z;
		float distance = sqrtf((absX * absX) + (absY * absY));

		switch (cfg.target_mode)
		{
		case closest_fov:
		{
			continue;
		}

		case closest:
		{
			if (distance < closest_dst)
			{
				closest_dst = distance;
				break;
			}

			continue;
		}

		case danger:
		{
			continue;
		}

		case lowest:
		{
			if (player_list[x]->health < lowest_hp)
			{
				lowest_hp = player_list[x]->health;
				break;
			}

			continue;
		}
		}

		final_yaw   = (float)(atan2f(absY, absX) * (180 / M_PI) - 90);
		final_pitch = (float)(atan2f(absZ, distance) * (180 / M_PI));
	}
	
	if (final_yaw)
	{
		local_player->yaw   = final_yaw;
		local_player->pitch = final_pitch;
	}
}

static int SetSpread(const int spread_value, player_entity* player_ent)
{
	if (player_ent != local_player) return spread_value;

	if (player_ent->equiped_wpn->weapon_id == shotgun_id) return spread_value * (cfg.shotgun_spread / 100);
	else return spread_value * (cfg.reg_spread / 100);
}

__declspec(naked) int SpreadDispatch()
{
	__asm
	{
		call [eax+4]   // original VF call 
		mov edx, [edi] // instruction overwritten by hook
		push [esp+4]   // [esp+4] = player entity
		push eax       // eax = spread value from VF
		call SetSpread
		add esp, 8
		ret
	}
}