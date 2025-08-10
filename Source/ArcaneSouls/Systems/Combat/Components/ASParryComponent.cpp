// ======================================================================
//  ASParryComponent.cpp   –   Arcane Souls 퍼펙트 패링 판정 컴포넌트
// ======================================================================

#include "ASParryComponent.h"
#include "ArcaneSouls/Characters/ASCharacterBase.h"
#include "ArcaneSouls/Systems/Combat/Data/ASParryConstants.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceDebug.h"
#include "Logging/LogMacros.h"
DEFINE_LOG_CATEGORY_STATIC(LogAS_Combat, Log, All);

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
	if (!GetWorld()) return;

	const float InLen  = (InWindow.EndTime - InWindow.StartTime);
	const float UseLen = FMath::Min(InLen, ASParry::PerfectParryWindow);

	// 절대시간 그대로 사용
	CurrentParryWindow.StartTime = InWindow.StartTime;
	CurrentParryWindow.EndTime   = CurrentParryWindow.StartTime + UseLen;
	CurrentParryWindow.Element   = InWindow.Element;

	bParryWindowActive = true;

	// 기존 예약 제거 후 새 예약
	GetWorld()->GetTimerManager().ClearTimer(ParryWindowTimerHandle);
	ScheduleParryWindowClear(CurrentParryWindow.EndTime);

	UE_LOG(LogAS_Combat, Display, TEXT("[Parry] Window Activated: Active=%s Start=%.4f End=%.4f Len=%.4f Elem=%d"),
		TEXT("true"),
		CurrentParryWindow.StartTime, CurrentParryWindow.EndTime, UseLen, (int32)CurrentParryWindow.Element);
#if !(UE_BUILD_SHIPPING)
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Orange,
			FString::Printf(TEXT("ParryWindow ON [%.3f~%.3f]"), CurrentParryWindow.StartTime, CurrentParryWindow.EndTime));
	}
#endif
}

void UASParryComponent::ScheduleParryWindowClear(float EndTimeSeconds)
{
	if (!GetWorld()) return;

	const float Now = GetWorld()->GetTimeSeconds();
	const float Delay = FMath::Max(0.f, EndTimeSeconds - Now);

	GetWorld()->GetTimerManager().SetTimer(
		ParryWindowTimerHandle,
		this,
		&UASParryComponent::OnParryWindowTimerExpired,
		Delay,
		false
	);

	UE_LOG(LogAS_Combat, Verbose, TEXT("[Parry] Clear scheduled in %.4f sec"), Delay);
}

void UASParryComponent::OnParryWindowTimerExpired()
{
	// 타이머 만료로 윈도우 종료
	ClearParryWindow();
}

void UASParryComponent::ClearParryWindow()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ParryWindowTimerHandle);
	}

	bParryWindowActive = false;
	CurrentParryWindow = FParryWindow{};

	UE_LOG(LogAS_Combat, Display, TEXT("[Parry] Window Cleared"));
}

// UASParryComponent.cpp
void UASParryComponent::EvaluateParry()
{
	const float Now = UGameplayStatics::GetTimeSeconds(this);

	UE_LOG(LogAS_Combat, Display, TEXT("[Parry] Evaluate: Active=%s Now=%.4f Window=[%.4f, %.4f] Elem=%d"),
		bParryWindowActive ? TEXT("true") : TEXT("false"),
		Now, CurrentParryWindow.StartTime, CurrentParryWindow.EndTime, (int32)CurrentParryWindow.Element);

	if (!bParryWindowActive)
	{
		HandleParryFail();
		return;
	}

	const bool bIsPerfect = CurrentParryWindow.IsInWindow(Now);
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
