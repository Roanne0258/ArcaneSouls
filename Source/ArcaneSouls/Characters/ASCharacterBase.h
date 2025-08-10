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

public: // UPROPERTY (public)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    UCameraComponent* FollowCamera;

public: // UFUNCTION (public)
    AASCharacterBase();

    // [Combat] Finisher/MP
    virtual void ReceiveFinisher(AActor* FinisherSource);
    virtual bool HasMP(int32 Amount) const;
    virtual void ConsumeMP(int32 Amount);
    virtual void RestoreMP(float Amount);

    // [Guard] 시작/종료
    UFUNCTION(BlueprintCallable, Category="Combat|Guard")
    void OnGuardStart();

    UFUNCTION(BlueprintCallable, Category="Combat|Guard")
    void OnGuardEnd();

    UFUNCTION(BlueprintCallable, Category="Combat|Guard")
    void OnGuardBlockHit(float DamageAmount, AActor* InstigatorActor);

    // 기존 시그니처 유지 시, 내부에서 OnGuardStart 호출하도록 연결 가능
    UFUNCTION(BlueprintCallable, Category="Combat|Guard")
    void ApplyGuard(float Unused, AActor* InstigatorActor);

    // [Stats]
    UFUNCTION(BlueprintPure, Category="Stats")
    float GetCurrentMP() const;

    UFUNCTION(BlueprintCallable, Category="Stats")
    void ModifyMP(float Delta);

    UFUNCTION(BlueprintPure, Category="Stats")
    virtual float GetMaxHP() const;

    UFUNCTION(BlueprintCallable, Category="Stats")
    virtual void ApplyChipDamage(float Amount);

protected: // UPROPERTY (protected)
    // Guard 중복 결제 방지
    UPROPERTY(VisibleInstanceOnly, Category="Combat|Guard")
    bool bIsGuarding = false;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UASParryComponent* ParryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UASFinisherManagerComponent* FinisherComponent;

    // Stats
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stats")
    float MaxMP = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats")
    float CurrentMP = 100.f;
};