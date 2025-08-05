// ======================================================================
//  ParryTypes.h   –   Arcane Souls 퍼펙트 패링 구조체 및 속성 Enum
// ======================================================================

#pragma once

#include "ParryTypes.generated.h"

UENUM(BlueprintType)
enum class EParryElementType : uint8
{
	None      UMETA(DisplayName="None"),
	Fire      UMETA(DisplayName="Fire"),
	Ice       UMETA(DisplayName="Ice"),
	Lightning UMETA(DisplayName="Lightning")
};

USTRUCT(BlueprintType)
struct FParryWindow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EndTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EParryElementType Element = EParryElementType::None;

	FParryWindow() = default;

	bool IsInWindow(float ElapsedTime) const
	{
		return ElapsedTime >= StartTime && ElapsedTime <= EndTime;
	}
};
