// ======================================================================
//  ASParryConstants.h   –   Arcane Souls 패링/가드 시스템 상수
// ======================================================================

#pragma once

namespace ASParry
{
	constexpr int32 PerfectParryWindowFrames = 2;
	constexpr float FrameDuration = 1.0f / 60.0f;
	constexpr float PerfectParryWindow = PerfectParryWindowFrames * FrameDuration; // ≈ 0.033s

	constexpr int32 GuardMPCost = 30;
	constexpr float GuardChipRatio = 0.005f;
}
