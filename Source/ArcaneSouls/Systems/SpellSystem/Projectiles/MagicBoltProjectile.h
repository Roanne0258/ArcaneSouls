// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MagicBoltProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ARCANESOULS_API AMagicBoltProjectile : public AASProjectileBase
{
	GENERATED_BODY()
public:
    AMagicBoltProjectile();

    virtual void HandleImpact(const FHitResult& Hit) override
    {
        // 폭발 이펙트 호출 등
        Super::HandleImpact(Hit);
    }

protected:
    virtual void BeginPlay() override;
};
