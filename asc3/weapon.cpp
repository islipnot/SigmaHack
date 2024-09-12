#include "pch.h"
#include "input.hpp"
#include "weapon.hpp"
#include "config.hpp"
#include "entities.hpp"
#include "pScanning.hpp"

UINT* player_count;
player_entity*** player_list_ptr;
player_entity* local_player;

static float GetAngleDistance(const player_entity* const self, float final_yaw, float final_pitch)
{
	float yaw_dst = std::abs(self->yaw - final_yaw);
	const float pitch_dst = std::abs(self->pitch - final_pitch);

	if (yaw_dst > 180.0f) 
		yaw_dst = std::abs(self->yaw - (360.0f - yaw_dst));

	return sqrtf(powf(yaw_dst, 2.0f) + powf(pitch_dst, 2.0f));
}

static void GetAngleInfo(float& distance, float& yaw, float& pitch, const player_entity* const self, const player_entity* const target)
{
	const float absX = self->x - target->x;
	const float absY = self->y - target->y;
	const float absZ = target->z - self->z;

	distance = sqrtf(powf(absX, 2.0f) + powf(absY, 2.0f));
	pitch = static_cast<float>(atan2f(absZ, distance) * (180.0f / M_PI));
	yaw = static_cast<float>(atan2f(absY, absX) * (180.0f / M_PI) - 90.0f);

	if (yaw < 0.0f) yaw += 360.0f;
}

void aimbot() // NEEDS TO ADJUST TO CROUCHING PLAYERS
{
	player_entity** const player_list = *player_list_ptr;

	constexpr float max_float = 18446744073709551615.0f;
	float closest_dst = max_float;
	float closest_aim = max_float;
	float most_danger = max_float;
	UINT lowest_hp = 1000;

	float final_yaw   = 0.0f;
	float final_pitch = 0.0f;

	for (int x = 1; x < *player_count; ++x)
	{
		if (player_list[x]->health > 100 || (player_list[x]->team == local_player->team && !cfg.target_team)) 
			continue;

		float angle_dst, distance, new_yaw, new_pitch;
		GetAngleInfo(distance, new_yaw, new_pitch, local_player, player_list[x]);
		angle_dst = GetAngleDistance(local_player, new_yaw, new_pitch);

		if (angle_dst > cfg.aimbot_fov)
			continue;

		switch (cfg.target_mode)
		{
		case closest_fov:
		{
			if (angle_dst < closest_aim)
			{
				closest_aim = angle_dst;
				break;
			}

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
			float targ_new_yaw, targ_new_pitch;
			GetAngleInfo(distance, targ_new_yaw, targ_new_pitch, player_list[x], local_player);
			angle_dst = GetAngleDistance(player_list[x], targ_new_yaw, targ_new_pitch);

			if (angle_dst < most_danger)
			{
				most_danger = angle_dst;
				break;
			}

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

		final_yaw = new_yaw;
		final_pitch = new_pitch;
	}
	
	if (final_yaw)
	{
		local_player->yaw = final_yaw;
		local_player->pitch = final_pitch;
	}
}

static int SetSpread(const int spread_value, const player_entity* const player_ent)
{
	if (player_ent != local_player || !cfg.adjust_spread) 
		return spread_value;

	if (player_ent->equiped_wpn->weapon_id == shotgun_id) 
		return spread_value * (cfg.shotgun_spread / 100.0f);
	else 
		return spread_value * (cfg.reg_spread / 100.0f);
}

__declspec(naked) int SpreadDispatch()
{
	__asm
	{
		call [eax+4]   // original VF call 
		mov edx, [edi] // instruction overwritten by hook
		push edx       // edx = player ent
		push [esp+8]   // [esp+4] = player entity
		push eax       // eax = spread value from VF
		call SetSpread
		add esp, 8
		pop edx
		ret
	}
}

void SetRecoil(const float recoil)
{
	switch (cfg.recoil_mode)
	{
	case visual:   { cfg.vis_recoil  = recoil; cfg.vis_recoil_mlt = recoil / 1000.0f; break; }
	case physical: { cfg.phys_recoil = recoil; break; }
	default:       { cfg.vis_recoil  = cfg.phys_recoil = recoil; }
	}
}