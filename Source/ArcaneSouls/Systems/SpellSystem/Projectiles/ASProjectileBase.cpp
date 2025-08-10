#include "ASProjectileBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
// AASProjectileBase.cpp (핵심 부분)
AASProjectileBase::AASProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;
	CollisionComp = CreateDefaultSubobject<USphereComponent>("Sphere");
	CollisionComp->InitSphereRadius(15.f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AASProjectileBase::OnHit);
	RootComponent = CollisionComp;

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("Move");
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->bRotationFollowsVelocity = true;

	InitialLifeSpan = 3.0f;
}

void AASProjectileBase::InitProjectile(float InSpeed, float InBaseDamage, float InDamageScale, EParryElementType InElement)
{
	MovementComp->InitialSpeed = InSpeed;
	MovementComp->MaxSpeed     = InSpeed;
	BaseDamage  = InBaseDamage;
	DamageScale = InDamageScale;
	Element     = InElement;
}

void AASProjectileBase::OnHit(UPrimitiveComponent* /*…*/, AActor* OtherActor, UPrimitiveComponent* /*…*/, FVector /*…*/, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != GetOwner())
	{
		ApplyDamage(OtherActor);
		HandleImpact(Hit);
		Destroy();
	}
}

void AASProjectileBase::HandleImpact(const FHitResult& Hit)
{
}

void AASProjectileBase::ApplyDamage(AActor* Other)
{
	
}

void AASProjectileBase::InitVelocity(const FVector& Velocity)
{
    if (MovementComp)
    {
        MovementComp->Velocity = Velocity;
    }
}

void AASProjectileBase::SetDamage(float NewDamage)
{
	Damage = NewDamage;
}