// ======================================================================
//  ASFinisherManagerComponent.cpp   –   Arcane Souls 피니셔 컷씬 매니저
// ======================================================================

#include "ASFinisherManagerComponent.h"
#include "ArcaneSouls/Characters/ASCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

UASFinisherManagerComponent::UASFinisherManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UASFinisherManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<AASCharacterBase>(GetOwner());
}

void UASFinisherManagerComponent::PlayFinisher(AActor* Target, AActor* Instigator)
{
	if (!OwnerCharacter || !FinisherMontage) return;

	CurrentTarget = Target;

	// 1. 프리즈 + 슬로우모션
	StartTimeDilation();

    // 2. 지연 후 몽타주 시작 (타이밍 파라미터)
    GetWorld()->GetTimerManager().SetTimer(
        FinisherTimer, this, &UASFinisherManagerComponent::PlayMontage, MontageDelaySec, false
    );
}

void UASFinisherManagerComponent::StartTimeDilation()
{
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), GlobalTimeDilation);
	if (OwnerCharacter && OwnerCharacter->GetMesh())
	{
        OwnerCharacter->CustomTimeDilation = PlayerTimeDilation; // 플레이어만 더 느리게
	}

	// 오디오 Duck 등은 Audio Manager에서 처리 (🛠 업데이트 예정)
}

void UASFinisherManagerComponent::PlayMontage()
{
	UAnimInstance* AnimInst = OwnerCharacter ? OwnerCharacter->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInst || !FinisherMontage) return;

	AnimInst->Montage_Play(FinisherMontage);
	AnimInst->Montage_JumpToSection(FName("Finisher_0"), FinisherMontage);

    // 종료 예약 (파라미터)
    GetWorld()->GetTimerManager().SetTimer(
        FinisherTimer, this, &UASFinisherManagerComponent::EndFinisher, CutsceneLengthSec, false
    );
}

void UASFinisherManagerComponent::EndFinisher()
{
	// 시간 복구
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
	if (OwnerCharacter)
	{
		OwnerCharacter->CustomTimeDilation = 1.f;
	}

	CurrentTarget = nullptr;

	// TODO: 카메라 복귀, 임팩트 연출 등 보완
}
