#include "MagicBoltProjectile.h"

#include "ArcaneSouls/Characters/Player/ASPlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"  // ApplyDamage 등 사용 시

AMagicBoltProjectile::AMagicBoltProjectile()
{
	// Tick 불필요
	PrimaryActorTick.bCanEverTick = false;

	// 1) 충돌 컴포넌트
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.f);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComp->OnComponentHit.AddDynamic(this, &AMagicBoltProjectile::OnHit);
	RootComponent = CollisionComp;

	// 2) 투사체 무브먼트 컴포넌트
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->InitialSpeed = 2500.f;
	MovementComp->MaxSpeed     = 2500.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bShouldBounce = false;

	// 3) 기타 기본값
	BaseDamage      = 10.f;
	DamageScale     = 1.0f;
	InitialLifeSpan = 3.0f;
}

void AMagicBoltProjectile::BeginPlay()
{
	Super::BeginPlay();
	// 필요 시 추가 초기화
}

void AMagicBoltProjectile::OnHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (OtherActor && OtherActor != GetOwner())
	{
		// 데미지 계산
		float PlayerMagicPower = 0.f;
		if (APawn* Inst = Cast<APawn>(GetOwner()))
			PlayerMagicPower = Cast<AASPlayerCharacter>(Inst)->GetMagicPower();

		float FinalDamage = BaseDamage + PlayerMagicPower * DamageScale;
		UGameplayStatics::ApplyDamage(OtherActor, FinalDamage, GetInstigatorController(),
									  this, UDamageType::StaticClass());

		// 이펙트 재생 로직 등 필요 시 HandleImpact 호출
		Destroy();
	}
}
