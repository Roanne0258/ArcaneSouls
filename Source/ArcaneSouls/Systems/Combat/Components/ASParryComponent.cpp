// ======================================================================
//  ASParryComponent.cpp   –   Arcane Souls 퍼펙트 패링 판정 컴포넌트
// ======================================================================

#include "ASParryComponent.h"
#include "ArcaneSouls/Characters/ASCharacterBase.h"
#include "ArcaneSouls/Systems/Combat/Data/ASParryConstants.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceDebug.h"

UASParryComponent::UASParryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UASParryComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<AASCharacterBase>(GetOwner());
}

void UASParryComponent::SetParryWindow(const FParryWindow& InWindow)
{
	const float WorldTime = UGameplayStatics::GetTimeSeconds(this);
	CurrentParryWindow = InWindow;

	CurrentParryWindow.StartTime = WorldTime; // 절대 시간
	CurrentParryWindow.EndTime   = WorldTime + (InWindow.EndTime - InWindow.StartTime); // 절대 시간

	bParryWindowActive = true;
}

void UASParryComponent::ClearParryWindow()
{
	bParryWindowActive = false;
	CurrentParryWindow = FParryWindow{};
}

void UASParryComponent::EvaluateParry()
{
	if (!bParryWindowActive)
	{
		HandleParryFail();
		return;
	}

	const float ElapsedTime = UGameplayStatics::GetTimeSeconds(this);
	const bool bIsPerfect = CurrentParryWindow.IsInWindow(ElapsedTime);
	bIsPerfect ? HandleParrySuccess() : HandleParryFail();
}

void UASParryComponent::HandleParrySuccess()
{
	UE_LOG(LogTemp, Log, TEXT("[Parry] Perfect Success"));

	if (OwnerCharacter)
	{
		OwnerCharacter->ReceiveFinisher(nullptr); // 공격자 필요시 전달
	}

	// TODO: GameplayTag 부여, Debuff 처리 등
	ClearParryWindow();
}

void UASParryComponent::HandleParryFail()
{
	UE_LOG(LogTemp, Warning, TEXT("[Parry] Failed → Guard if held"));

	// 조기 가드 처리 등은 ASCharacterBase::ApplyGuard()에 위임
	if (OwnerCharacter)
	{
		OwnerCharacter->ApplyGuard(0.f, nullptr);
	}

	ClearParryWindow();
}
