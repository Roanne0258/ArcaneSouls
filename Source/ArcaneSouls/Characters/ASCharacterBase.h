// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ASCharacterBase.generated.h"

class AActor;
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
	virtual void ReceiveFinisher(AActor* FinisherSource);
	virtual bool HasMP(int32 Amount) const;
	virtual void ConsumeMP(int32 Amount);
	virtual void RestoreMP(float Amount);

	// Guard 시작/종료(입력 바인딩에서 호출)
	UFUNCTION(BlueprintCallable) void OnGuardStart();
	UFUNCTION(BlueprintCallable) void OnGuardEnd();

	UFUNCTION(BlueprintCallable) void OnGuardBlockHit(float DamageAmount, AActor* Instigator);

	// 기존 시그니처 유지 시, 내부에서 OnGuardStart 호출하도록 연결 가능
	UFUNCTION(BlueprintCallable) void ApplyGuard(float /*Unused*/, AActor* /*Instigator*/);

	// MP/HP 접근·수정(이미 있다면 재사용)
	UFUNCTION(BlueprintPure)  float GetCurrentMP() const;
	UFUNCTION(BlueprintCallable) void ModifyMP(float Delta);
	UFUNCTION(BlueprintPure)  float GetMaxHP() const;
	UFUNCTION(BlueprintCallable) void ApplyChipDamage(float Amount);

protected:
	// Guard 중복 결제 방지
	UPROPERTY(VisibleInstanceOnly, Category="Guard")
	bool bIsGuarding = false;
	
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