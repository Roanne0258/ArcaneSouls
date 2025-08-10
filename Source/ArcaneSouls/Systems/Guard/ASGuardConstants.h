#pragma once
namespace ASGuard
{
	inline constexpr float GuardStartMPCost = 10.f;   // 가드 '켜질 때' 1회
	inline constexpr float GuardBlockMPCost = 20.f;   // 가드 중 '히트 1회'당
	inline constexpr float GuardChipRatio   = 0.005f; // 부족 1MP = MaxHP 0.5% 칩
}