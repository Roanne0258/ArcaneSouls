// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARCANESOULS_API IDamageableInterface
{
	GENERATED_BODY()

public:
	/** 그리드 데미지 처리 (플레이어 → 액터) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grid")
	void ApplyGridDamage(int32 Amount);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Damage")
	void ApplyDamage(int32 DamageAmount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Damage")
	int32 GetHealth() const;
};
