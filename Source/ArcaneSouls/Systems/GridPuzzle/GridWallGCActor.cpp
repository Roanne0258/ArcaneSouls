// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWallGCActor.h"

#include "GridFloorGCActor.h"
#include "Components/TextRenderComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

AGridWallGCActor::AGridWallGCActor()
{
	GCComp = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GCComp"));
	RootComponent = GCComp;              // 부모 MeshComp는 사용 안 함
	Mesh = nullptr;                      // 상속 포인터 무효화

	HealthText->SetupAttachment(RootComponent);
	HealthText->SetRelativeLocation({0,0,60});
}

void AGridWallGCActor::ApplyGridDamage_Implementation(int32 Amount)
{
	Health = FMath::Max(0, Health - Amount);
	RefreshVisual();

	if (Health <= 0)
	{
		Destroy(); // ← 벽은 무조건 파괴
		return;
	}

	// ±750 방향 바닥 검사 (벽 주변만)
	const FVector Center = GetActorLocation();
	const bool bHorz = FMath::IsNearlyZero(FMath::Fmod(GetActorRotation().Yaw, 180.f));
	const FVector Offset = bHorz ? FVector(0, 750.f, 0) : FVector(750.f, 0, 0);

	for (const FVector Dir : { Offset, -Offset })
	{
		FVector Pos = Center + Dir;
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(Hit, Pos + FVector(0,0,200), Pos - FVector(0,0,200), ECC_Visibility, Params))
		{
			if (AActor* Target = Hit.GetActor())
			{
				// ✅ 바닥만 타격
				if (Target->IsA<AGridFloorGCActor>() && Target->Implements<UDamageableInterface>())
				{
					IDamageableInterface::Execute_ApplyGridDamage(Target, Amount);
				}
			}
		}
	}

}
