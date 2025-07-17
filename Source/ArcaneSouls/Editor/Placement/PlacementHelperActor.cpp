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
	if (bAutoAlign) { AlignToGround(); }

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

void APlacementHelperActor::AlignSlope()      /* ★ NEW */
{
#if WITH_EDITOR
	if (!MeshComp) return;

	const FVector Start = MeshComp->GetComponentLocation();
	const FVector End   = Start - FVector(0,0,PlacementConst::TraceDepth);

	FHitResult Hit;
	FCollisionQueryParams P(SCENE_QUERY_STAT(AlignSlope), true);
	P.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, P))
	{
		/* 노멀 → Rotator (Yaw 제외) */
		FRotator SlopeRot = Hit.Normal.Rotation();     // Pitch·Roll 포함
		if (!bAlignYawToSlope)
		{
			SlopeRot.Yaw = MeshComp->GetComponentRotation().Yaw;  // Yaw 유지
		}
		MeshComp->SetWorldRotation(SlopeRot);
		UE_LOG(LogASPlacement, Log, TEXT("AlignSlope to %.1f, %.1f"), 
			   SlopeRot.Pitch, SlopeRot.Roll);
	}
#endif
}

void APlacementHelperActor::RandomYaw()        /* ★ NEW */
{
#if WITH_EDITOR
	if (!MeshComp) return;
	float Angle = FMath::FRandRange(0.f, 360.f);
	if (RandomYawStep > KINDA_SMALL_NUMBER)
	{
		Angle = FMath::GridSnap(Angle, RandomYawStep);
	}
	FRotator R = MeshComp->GetComponentRotation();
	R.Yaw = Angle;
	MeshComp->SetWorldRotation(R);
#endif
}

// 무작위 위치 이동
void APlacementHelperActor::RandomLocationInRadius()
{
#if WITH_EDITOR
	if (!MeshComp) return;

	FVector Curr   = MeshComp->GetComponentLocation();
	FVector2D Rand = FMath::RandPointInCircle(RandomRadius);   // XY 평면
	Curr.X += Rand.X;
	Curr.Y += Rand.Y;
	MeshComp->SetWorldLocation(Curr);

	UE_LOG(LogASPlacement, Log, TEXT("RandomLocation Δ(%.0f, %.0f)"),
		   Rand.X, Rand.Y);
#endif
}

// 위치 초기화
void APlacementHelperActor::ResetLocation()
{
#if WITH_EDITOR
	if (MeshComp)
	{
		MeshComp->SetWorldLocation(ResetLocationOrigin);
		UE_LOG(LogASPlacement, Log, TEXT("ResetLocation → %s"),
			   *ResetLocationOrigin.ToString());
	}
#endif
}

// 회전 초기화
void APlacementHelperActor::ResetRotation()
{
#if WITH_EDITOR
	if (MeshComp)
	{
		MeshComp->SetWorldRotation(ResetRotationValue);
		UE_LOG(LogASPlacement, Log, TEXT("ResetRotation → %s"),
			   *ResetRotationValue.ToString());
	}
#endif
}

/* ──────────────── Material Cycler ─────────────── */
void APlacementHelperActor::CycleMaterial()
{
#if WITH_EDITOR
	if (!MeshComp || MaterialList.Num() == 0) return;

	CurrentMatIdx = (CurrentMatIdx + 1) % MaterialList.Num();
	MeshComp->SetMaterial(0, MaterialList[CurrentMatIdx]);

	UE_LOG(LogASPlacement, Log,
		TEXT("MaterialCycler → %s"),
		*MaterialList[CurrentMatIdx]->GetName());
#endif
}

/* ──────────────── Collision Toggle ───────────── */
void APlacementHelperActor::ToggleCollision()
{
#if WITH_EDITOR
	if (!MeshComp || CollisionCycle.Num() == 0) return;

	CurrentCollIdx = (CurrentCollIdx + 1) % CollisionCycle.Num();
	MeshComp->SetCollisionEnabled(CollisionCycle[CurrentCollIdx]);

	static const TCHAR* StateTxt[] = { TEXT("No"), TEXT("Overlap"), TEXT("Block") };
	UE_LOG(LogASPlacement, Log, TEXT("CollisionToggle → %s"),
		StateTxt[CurrentCollIdx]);
#endif
}
