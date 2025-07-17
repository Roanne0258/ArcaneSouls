// PlacementHelperActor.cpp
#include "PlacementHelperActor.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogASPlacement);

namespace PlacementConst { constexpr float TraceDepth = 10000.f; }

APlacementHelperActor::APlacementHelperActor()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetMobility(EComponentMobility::Movable);
	MeshComp->bEditableWhenInherited = true;
}

void APlacementHelperActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
#if WITH_EDITOR
	if (bAutoAlign)        // NEW
	{
		AlignToGround();
	}
#endif
}

void APlacementHelperActor::AlignToGround()
{
#if WITH_EDITOR
	if (!MeshComp) return;

	const FVector Start = MeshComp->GetComponentLocation();
	const FVector End   = Start - FVector(0.f, 0.f, PlacementConst::TraceDepth);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AlignToGround), true);
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
	{
		float DeltaZ = 0.f;

		if (bUseMeshBottom)   // NEW: 메시 하단 기준
		{
			const FBoxSphereBounds Bounds = MeshComp->Bounds;
			const float MeshBottomZ = Bounds.Origin.Z - Bounds.BoxExtent.Z;
			DeltaZ = Hit.Location.Z - MeshBottomZ;
		}
		else                  // 피벗 기준 (기존 방식)
		{
			DeltaZ = Hit.Location.Z - Start.Z;
		}

		FVector NewLoc = Start;
		NewLoc.Z += DeltaZ;
		MeshComp->SetWorldLocation(NewLoc);

		UE_LOG(LogASPlacement, Log, TEXT("Aligned (%s). ΔZ=%.1f cm"),
			bUseMeshBottom ? TEXT("Bottom") : TEXT("Pivot"), DeltaZ);
#if !(UE_BUILD_SHIPPING)
		DrawDebugLine(GetWorld(), Start, Hit.Location, FColor::Green, false, 1.f);
#endif
	}
#endif
}
