// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ArcaneSouls/Systems/Combat/Data/ParryTypes.h"
#include "AnimNotifyState_ParryWindow.generated.h"

/* ───── Forward Decls ───── */
class UASParryComponent;

UCLASS()
class ARCANESOULS_API UAnimNotifyState_ParryWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	// Variables
	UPROPERTY(EditAnywhere, Category="Parry")
	float StartTime = 0.f;

	UPROPERTY(EditAnywhere, Category="Parry")
	float EndTime = 0.f;

	UPROPERTY(EditAnywhere, Category="Parry")
	EParryElementType Element = EParryElementType::None;

public:
	// Functions
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
