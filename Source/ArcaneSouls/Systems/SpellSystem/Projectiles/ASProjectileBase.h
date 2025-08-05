// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ASProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
enum class EParryElementType : uint8;

UCLASS()
class ARCANESOULS_API AASProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:
	AASProjectileBase();

	/** 기본값 세팅 */
	UFUNCTION(BlueprintCallable, Category="Projectile")
	void InitProjectile(float InSpeed, float InBaseDamage, float InDamageScale, EParryElementType InElement);

protected:
	// ■ 콜리전
	UPROPERTY(VisibleAnywhere) USphereComponent* CollisionComp;
	// ■ 움직임
	UPROPERTY(VisibleAnywhere) UProjectileMovementComponent* MovementComp;
	// ■ 데미지/속성
	UPROPERTY(EditDefaultsOnly, Category="Damage") float BaseDamage;
	UPROPERTY(EditDefaultsOnly, Category="Damage") float DamageScale;
	UPROPERTY(EditDefaultsOnly, Category="Damage") EParryElementType Element;

	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	/** 파생 클래스에서 이펙트·추가 로직 구현 */
	virtual void HandleImpact(const FHitResult& Hit);

private:
	void ApplyDamage(AActor* Other);
};
