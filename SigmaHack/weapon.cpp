#include "pch.hpp"
#include "input.hpp"
#include "weapon.hpp"
#include "config.hpp"
#include "entities.hpp"

UINT* PlayerCount;
PlayerEnt*** pPlayerList;
PlayerEnt* LocalPlayer;

float GetAngleDistance(const PlayerEnt* self, float FinalYaw, float FinalPitch)
{
	float YawDst = std::abs(self->yaw - FinalYaw);
	float PitchDst = std::abs(self->pitch - FinalPitch);

	if (YawDst > 180.0f) 
		YawDst = std::abs(self->yaw - (360.0f - YawDst));

	return sqrtf(powf(YawDst, 2.0f) + powf(PitchDst, 2.0f));
}

void GetAngleInfo(float& distance, float& yaw, float& pitch, const PlayerEnt* self, const PlayerEnt* target)
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
	PlayerEnt** PlayerList = *pPlayerList;

	constexpr float MaxFloat = 18446744073709551615.0f;
	float ClosestDst = MaxFloat;
	float ClosestAim = MaxFloat;
	float MostDanger = MaxFloat;
	UINT LowestHp = 1000;

	float FinalYaw   = 0.0f;
	float FinalPitch = 0.0f;

	for (UINT x = 1; x < *PlayerCount; ++x)
	{
		if (PlayerList[x]->health > 100 || (PlayerList[x]->team == LocalPlayer->team && !cfg.TargetTeam)) 
			continue;

		float AngleDst, distance, NewYaw, NewPitch;
		GetAngleInfo(distance, NewYaw, NewPitch, LocalPlayer, PlayerList[x]);
		AngleDst = GetAngleDistance(LocalPlayer, NewYaw, NewPitch);

		if (AngleDst > cfg.AimbotFOV)
			continue;

		switch (cfg.TargetMode)
		{
		case ClosestFOV:
		{
			if (AngleDst < ClosestAim)
			{
				ClosestAim = AngleDst;
				break;
			}

			continue;
		}

		case closest:
		{
			if (distance < ClosestDst)
			{
				ClosestDst = distance;
				break;
			}

			continue;
		}

		case danger:
		{
			float targ_new_yaw, targ_new_pitch;
			GetAngleInfo(distance, targ_new_yaw, targ_new_pitch, PlayerList[x], LocalPlayer);
			AngleDst = GetAngleDistance(PlayerList[x], targ_new_yaw, targ_new_pitch);

			if (AngleDst < MostDanger)
			{
				MostDanger = AngleDst;
				break;
			}

			continue;
		}

		case lowest:
		{
			if (PlayerList[x]->health < LowestHp)
			{
				LowestHp = PlayerList[x]->health;
				break;
			}

			continue;
		}
		}

		FinalYaw = NewYaw;
		FinalPitch = NewPitch;
	}
	
	if (FinalYaw)
	{
		LocalPlayer->yaw = FinalYaw;
		LocalPlayer->pitch = FinalPitch;
	}
}

int SetSpread(int SpreadValue, const PlayerEnt* PlayerEnt)
{
	if (PlayerEnt != LocalPlayer || !cfg.AdjustSpread) 
		return SpreadValue;

	if (PlayerEnt->equiped_wpn->weapon_id == shotgun_id)
	{
		return SpreadValue * static_cast<int>(cfg.ShotgunSpread / 100.0f);
	}
	else return SpreadValue * static_cast<int>(cfg.RegSpread / 100.0f);
}

__declspec(naked) int __cdecl SpreadDispatch()
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

void SetRecoil(float recoil)
{
	switch (cfg.RecoilMode)
	{
	case visual:
	{
		cfg.VisRecoil = recoil; 
		cfg.VisRecoilMulti = recoil / 1000.0f; 
		break;
	}

	case physical:
	{
		cfg.PhysRecoil = recoil;
		break;
	}

	default: cfg.VisRecoil = cfg.PhysRecoil = recoil;
	
	}
}