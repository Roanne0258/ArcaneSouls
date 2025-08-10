// ======================================================================
//  BossBaseCharacter.cpp   –   Arcane Souls 보스 캐릭터 베이스 클래스
// ======================================================================

#include "BossBaseCharacter.h"
#include "ArcaneSouls/Systems/Combat/Components/ASFinisherManagerComponent.h"
#include "ArcaneSouls/Systems/Combat/Data/ParryTypes.h"
#include "ArcaneSouls/Systems/Combat/Data/ASParryConstants.h"
#include "Kismet/GameplayStatics.h"
#include "Slate/SGameLayerManager.h"

ABossBaseCharacter::ABossBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABossBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 시작 시 HP 동기화
	CurrentHP = MaxHP;
}

void ABossBaseCharacter::ReceiveFinisher(AActor* FinisherSource)
{
	Super::ReceiveFinisher(FinisherSource);

	OnFinisherHit();
}

void ABossBaseCharacter::OnFinisherHit()
{
    // 보스 Max HP 자체를 15% 감소시키고, 현재 HP를 새로운 Max에 맞춰 클램프
    MaxHP = FMath::Max(1.f, MaxHP * 0.85f);
    CurrentHP = FMath::Min(CurrentHP, MaxHP);

	// 디버프 적용 (예시: 최근 ParryWindow 정보에서 가져온다고 가정)
	// 실제 적용은 Notify → ParryComponent → Boss에 전달 구조 필요
	ApplyElementDebuff(EParryElementType::Ice); // 기본은 Ice (실제 연동 필요)
}

void ABossBaseCharacter::ApplyElementDebuff(EParryElementType Element)
{
	if (DebuffDurations.Contains(Element))
	{
		// 중첩 시간 증가
		DebuffDurations[Element] += 60.f;
	}
	else
	{
		DebuffDurations.Add(Element, 60.f);
	}

	ApplyDebuffEffect(Element);
}

void ABossBaseCharacter::ApplyDebuffEffect(EParryElementType Type)
{
	switch (Type)
	{
	case EParryElementType::Fire:
		// TODO: DOT Tick
			break;

	case EParryElementType::Ice:
		// TODO: Movement Slow
			break;

	case EParryElementType::Lightning:
		// TODO: Stun 적용
			bIsStunned = true;
		break;

	default:
		break;
	}
}

float ABossBaseCharacter::GetMaxHP() const
{
    return MaxHP;
}

void ABossBaseCharacter::ApplyChipDamage(float Amount)
{
    CurrentHP = FMath::Clamp(CurrentHP - Amount, 0.f, MaxHP);
}