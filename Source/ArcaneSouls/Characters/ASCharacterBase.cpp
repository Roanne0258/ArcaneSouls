// Fill out your copyright notice in the Description page of Project Settings.


#include "ASCharacterBase.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ArcaneSouls/Systems/Combat/Components/ASParryComponent.h"
#include "ArcaneSouls/Systems/Combat/Components/ASFinisherManagerComponent.h"
#include "ArcaneSouls/Systems/Combat/Data/ASParryConstants.h"
#include "Kismet/GameplayStatics.h"

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

void AASCharacterBase::TryParry()
{
	if (ParryComponent)
	{
		ParryComponent->EvaluateParry();
	}
}

void AASCharacterBase::ApplyGuard(float Damage, AActor* DamageCauser)
{
	const int32 Cost = ASParry::GuardMPCost;
	if (HasMP(Cost))
	{
		ConsumeMP(Cost);
	}
	else
	{
		const float Deficit = Cost - CurrentMP;
		ConsumeMP(CurrentMP);
		const float HPChip = Deficit * ASParry::GuardChipRatio;
		// Chip damage 적용 로직 (🛠 업데이트 예정)
	}
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
