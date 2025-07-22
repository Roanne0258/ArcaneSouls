// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWallGCActor.h"

#include "GridFloorGCActor.h"
#include "GridPuzzleManagerComponent.h"
#include "Components/TextRenderComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "UObject/FastReferenceCollector.h"

AGridWallGCActor::AGridWallGCActor()
{
	GCComp = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GCComp"));
	RootComponent = GCComp;              // 부모 MeshComp는 사용 안 함
	Mesh = nullptr;                      // 상속 포인터 무효화

	HealthText->SetupAttachment(RootComponent);
	HealthText->SetRelativeLocation({0,0,60});
}

void AGridWallGCActor::BeginPlay()
{
	Super::BeginPlay();
	GridMgr = GetWorld()->GetFirstPlayerController()
				  ->GetPawn()
				  ->FindComponentByClass<UGridPuzzleManagerComponent>();
}

void AGridWallGCActor::ApplyGridDamage_Implementation(int32 Amount)
{
	Health = FMath::Max(0, Health - Amount);
	RefreshVisual();

	// ⬇️ 반드시 GridMgr, GridA/B 값 필요 (리빌드매핑에서 할당됨)
	if (GridMgr)
	{
		// 두 끝 좌표를 GridMgr에서 직접 조회해 **FindFloorActorByGridCoord** 사용!
		for (const FIntPoint& Coord : { GridA, GridB })
		{
			if (AGridFloorGCActor* Floor = GridMgr->FindFloorActorByGridCoord(Coord))
			{
				Floor->ApplyGridDamage(Amount);
			}
		}
	}

	if (Health <= 0)
		Destroy();
}

void AGridWallGCActor::ApplyGridIce_Implementation(int32 Amount)
{
	if (IsActorBeingDestroyed() || !IsValid(this)) return; // UE5 권장!
	Health = FMath::Clamp(Health + Amount, 0, 3);
	RefreshVisual();
	UE_LOG(LogAS_GridPuzzle, Log, TEXT("Wall ICE: Health = %d"), Health);
}


