// Fill out your copyright notice in the Description page of Project Settings.


#include "GridFloorGCActor.h"
#include "Components/TextRenderComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

AGridFloorGCActor::AGridFloorGCActor()
{
	GCComp = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GCComp"));
	RootComponent = GCComp;              // 부모 MeshComp는 사용 안 함
	Mesh = nullptr;                      // 상속 포인터 무효화

	HealthText->SetupAttachment(RootComponent);
	HealthText->SetRelativeLocation({0,0,60});
}

void AGridFloorGCActor::ApplyGridDamage_Implementation(int32 Amount)
{
	Health = FMath::Max(0, Health - Amount);
	RefreshVisual();

	if (Health <= 0)
	{
		Destroy(); // 필요시 GC로 교체 가능
	}
}
