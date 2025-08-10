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
	// [Parry] 패링 속성 타입 (엘리먼트)
	UPROPERTY(EditAnywhere, Category="Parry")
	EParryElementType Element = EParryElementType::None;

	// [Tuning] 노티 구간 내 패링 "중심" 위치 (0=시작, 0.5=중앙, 1=끝)
	UPROPERTY(EditAnywhere, Category="Parry", meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float ActivationPointNormalized = 0.5f;

	// [Tuning] 중심에서 추가로 더/덜 당기기(초). +면 뒤로, -면 앞으로.
	UPROPERTY(EditAnywhere, Category="Parry")
	float ActivationBiasSeconds = 0.f;

public:
	// Functions
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
