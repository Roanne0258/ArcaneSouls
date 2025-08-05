// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ASCharacterBase.generated.h"

class UASFinisherManagerComponent;
class UASParryComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ARCANESOULS_API AASCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	/* ───── Camera ───── */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	UCameraComponent* FollowCamera;

public:
	// Sets default values for this character's properties
	AASCharacterBase();

	/* ───── 전투 액션 ───── */
	virtual void TryParry();
	virtual void ApplyGuard(float Damage, AActor* DamageCauser);
	virtual void ReceiveFinisher(AActor* FinisherSource);
	virtual bool HasMP(int32 Amount) const;
	virtual void ConsumeMP(int32 Amount);
	virtual void RestoreMP(float Amount);

	/* ───── 스탯 조회 ───── */
	FORCEINLINE float GetMaxMP() const { return MaxMP; }
	FORCEINLINE float GetCurrentMP() const { return CurrentMP; }

protected:
	/* ───── 컴포넌트 ───── */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UASParryComponent* ParryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UASFinisherManagerComponent* FinisherComponent;

	/* ───── 스탯 ───── */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stats")
	float MaxMP = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats")
	float CurrentMP = 100.f;
};