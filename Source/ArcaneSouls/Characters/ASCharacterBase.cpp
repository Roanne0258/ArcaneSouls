// Fill out your copyright notice in the Description page of Project Settings.


#include "ASCharacterBase.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ArcaneSouls/Systems/Combat/Components/ASParryComponent.h"
#include "ArcaneSouls/Systems/Combat/Components/ASFinisherManagerComponent.h"
#include "ArcaneSouls/Systems/Combat/Data/ASParryConstants.h"
#include "Kismet/GameplayStatics.h"
#include "ArcaneSouls/Core/ASLogChannels.h"
#include "ArcaneSouls/Systems/Guard/ASGuardConstants.h"

// Sets default values
AASCharacterBase::AASCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	ParryComponent = CreateDefaultSubobject<UASParryComponent>(TEXT("ParryComponent"));
	FinisherComponent = CreateDefaultSubobject<UASFinisherManagerComponent>(TEXT("FinisherComponent"));

}

void AASCharacterBase::OnGuardStart()
{
	if (bIsGuarding) return;

	const float cost    = ASGuard::GuardStartMPCost;  // ← 10으로 변경
	const float mp      = GetCurrentMP();
	const float pay     = FMath::Min(mp, cost);
	const float deficit = cost - pay;

	if (pay > 0.f) { ModifyMP(-pay); }
	if (deficit > 0.f)
	{
		const float chip = GetMaxHP() * ASGuard::GuardChipRatio * deficit;
		ApplyChipDamage(chip);
	}

	bIsGuarding = true;
	UE_LOG(LogAS_Combat, Display, TEXT("[Guard] Start: pay=%.1f deficit=%.1f"), pay, deficit);
}

void AASCharacterBase::OnGuardEnd()
{
	if (!bIsGuarding) return;
	bIsGuarding = false;
	UE_LOG(LogAS_Combat, Display, TEXT("🛑 Guard Ended"));
	// TODO: 해제 비주얼/상태
}

void AASCharacterBase::OnGuardBlockHit(float DamageAmount, AActor* Instigator)
{
	if (!bIsGuarding) return;

	const float cost    = ASGuard::GuardBlockMPCost;   // ← 20
	const float mp      = GetCurrentMP();
	const float pay     = FMath::Min(mp, cost);
	const float deficit = cost - pay;

	if (pay > 0.f) { ModifyMP(-pay); }
	if (deficit > 0.f)
	{
		const float chip = GetMaxHP() * ASGuard::GuardChipRatio * deficit;
		ApplyChipDamage(chip);
	}

	UE_LOG(LogAS_Combat, Display, TEXT("[Guard] BlockedHit: pay=%.1f deficit=%.1f dmg=%.1f"), pay, deficit, DamageAmount);

	// 참고: 여기서 데미지 감쇠/경직 무효화 등 가드 효과를 함께 처리해도 좋아요.
}

void AASCharacterBase::ApplyGuard(float /*Unused*/, AActor* /*Instigator*/)
{
	OnGuardStart();
}

void AASCharacterBase::ReceiveFinisher(AActor* FinisherSource)
{
	if (FinisherComponent)
	{
		FinisherComponent->PlayFinisher(this, FinisherSource);
	}
}

bool AASCharacterBase::HasMP(int32 Amount) const
{
	return CurrentMP >= Amount;
}

void AASCharacterBase::ConsumeMP(int32 Amount)
{
	CurrentMP = FMath::Max(0.f, CurrentMP - Amount);
}

void AASCharacterBase::RestoreMP(float Amount)
{
	CurrentMP = FMath::Min(MaxMP, CurrentMP + Amount);
}

float AASCharacterBase::GetCurrentMP() const { return CurrentMP; }
void  AASCharacterBase::ModifyMP(float Delta) { CurrentMP = FMath::Clamp(CurrentMP + Delta, 0.f, MaxMP); }