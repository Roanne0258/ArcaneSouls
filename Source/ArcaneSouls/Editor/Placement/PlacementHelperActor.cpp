// PlacementHelperActor.cpp (with extended editor visual feedback)
#include "PlacementHelperActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
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
    if (bAutoAlign)
    {
        AlignToGround();
    }
#endif
}

/* ───────────────────────────────── AlignToGround ───────────────────────────── */
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
        if (bUseMeshBottom)
        {
            const FBoxSphereBounds Bounds = MeshComp->Bounds;
            const float MeshBottomZ = Bounds.Origin.Z - Bounds.BoxExtent.Z;
            DeltaZ = Hit.Location.Z - MeshBottomZ;
        }
        else
        {
            DeltaZ = Hit.Location.Z - Start.Z;
        }

        FVector NewLoc = Start; NewLoc.Z += DeltaZ;
        MeshComp->SetWorldLocation(NewLoc);

        // ── 시각 피드백 ──
        const FString Msg = FString::Printf(TEXT("[%s] AlignToGround ΔZ = %.1f cm"), *GetActorLabel(), DeltaZ);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetUniqueID()), 1.5f, FColor::Blue, Msg);
        }
        DrawDebugLine(GetWorld(), Start, Hit.Location, FColor::Green, false, 1.5f);
        DrawDebugPoint(GetWorld(), Hit.Location, 10.f, FColor::Emerald, false, 1.5f);

        UE_LOG(LogASPlacement, Log, TEXT("Aligned (%s). ΔZ=%.1f cm"), bUseMeshBottom ? TEXT("Bottom") : TEXT("Pivot"), DeltaZ);
    }
#endif
}

/* ───────────────────────────────── AlignSlope ─────────────────────────────── */
void APlacementHelperActor::AlignSlope()
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
        FRotator SlopeRot = Hit.Normal.Rotation();
        if (!bAlignYawToSlope)
        {
            SlopeRot.Yaw = MeshComp->GetComponentRotation().Yaw;
        }
        MeshComp->SetWorldRotation(SlopeRot);

        // ── 시각 피드백 ──
        const FString Msg = FString::Printf(TEXT("[%s] AlignSlope P=%.1f R=%.1f"), *GetActorLabel(), SlopeRot.Pitch, SlopeRot.Roll);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetUniqueID()), 1.5f, FColor::Purple, Msg);
        }
        DrawDebugLine(GetWorld(), Hit.Location, Hit.Location + Hit.Normal * 120.f, FColor::Magenta, false, 1.5f, 0, 2.f);

        UE_LOG(LogASPlacement, Log, TEXT("AlignSlope to %.1f, %.1f"), SlopeRot.Pitch, SlopeRot.Roll);
    }
#endif
}

/* ───────────────────────────────── RandomYaw ───────────────────────────────── */
void APlacementHelperActor::RandomYaw()
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

    const FString Msg = FString::Printf(TEXT("[%s] Yaw %.0f°"), *GetActorLabel(), Angle);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetUniqueID()), 1.5f, FColor::Orange, Msg);
    }

    const FVector Loc = MeshComp->GetComponentLocation();
    const FVector Dir = MeshComp->GetForwardVector() * 120.f;
    DrawDebugDirectionalArrow(GetWorld(), Loc, Loc + Dir, 80.f, FColor::Orange, false, 1.5f);

    UE_LOG(LogASPlacement, Log, TEXT("RandomYaw → %.0f°"), Angle);
#endif
}

/* ───────────────────────────── RandomLocationInRadius ─────────────────────── */
void APlacementHelperActor::RandomLocationInRadius()
{
#if WITH_EDITOR
    if (!MeshComp) return;

    const FVector PrevLoc = MeshComp->GetComponentLocation();
    FVector2D Rand = FMath::RandPointInCircle(RandomRadius);
    FVector NewLoc = PrevLoc + FVector(Rand.X, Rand.Y, 0.f);
    MeshComp->SetWorldLocation(NewLoc);

    // 시각 피드백
    if (GEngine)
    {
        const FString Msg = FString::Printf(TEXT("[%s] RandomLoc Δ(%.0f, %.0f)"), *GetActorLabel(), Rand.X, Rand.Y);
        GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetUniqueID()), 1.5f, FColor::Green, Msg);
    }
    DrawDebugSphere(GetWorld(), PrevLoc, RandomRadius, 16, FColor::Green, false, 1.5f);
    DrawDebugLine  (GetWorld(), PrevLoc, NewLoc, FColor::Green, false, 1.5f, 0, 2.f);

    UE_LOG(LogASPlacement, Log, TEXT("RandomLocation Δ(%.0f, %.0f)"), Rand.X, Rand.Y);
#endif
}

/* ───────────────────────────── ResetLocation / Rotation ───────────────────── */
void APlacementHelperActor::ResetLocation()
{
#if WITH_EDITOR
    if (MeshComp)
    {
        MeshComp->SetWorldLocation(ResetLocationOrigin);
        const FString Msg = FString::Printf(TEXT("[%s] ResetLocation"), *GetActorLabel());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetUniqueID()), 1.5f, FColor::White, Msg);
        }
        UE_LOG(LogASPlacement, Log, TEXT("ResetLocation → %s"), *ResetLocationOrigin.ToString());
    }
#endif
}

void APlacementHelperActor::ResetRotation()
{
#if WITH_EDITOR
    if (MeshComp)
    {
        MeshComp->SetWorldRotation(ResetRotationValue);
        const FString Msg = FString::Printf(TEXT("[%s] ResetRotation"), *GetActorLabel());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetUniqueID()), 1.5f, FColor::White, Msg);
        }
        UE_LOG(LogASPlacement, Log, TEXT("ResetRotation → %s"), *ResetRotationValue.ToString());
    }
#endif
}

/* ───────────────────────────── Material Cycler ────────────────────────────── */
void APlacementHelperActor::CycleMaterial()
{
#if WITH_EDITOR
    if (!MeshComp || MaterialList.Num() == 0) return;

    CurrentMatIdx = (CurrentMatIdx + 1) % MaterialList.Num();
    UMaterialInterface* NewMat = MaterialList[CurrentMatIdx];
    MeshComp->SetMaterial(0, NewMat);

    const FString Msg = FString::Printf(TEXT("[%s] Material → %s"), *GetActorLabel(), *NewMat->GetName());
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetUniqueID()), 1.5f, FColor::Turquoise, Msg);
    }
    DrawDebugString(GetWorld(), MeshComp->GetComponentLocation() + FVector(0,0,60.f), NewMat->GetName(), nullptr, FColor::Turquoise, 1.5f, false);

    UE_LOG(LogASPlacement, Log, TEXT("MaterialCycler → %s"), *NewMat->GetName());
#endif
}

/* ───────────────────────────── Collision Toggle ───────────────────────────── */
void APlacementHelperActor::ToggleCollision()
{
    if (!MeshComp) return;

    CurrentCollIdx = (CurrentCollIdx + 1) % CollisionCycle.Num();
    const ECollisionEnabled::Type NewMode = CollisionCycle[CurrentCollIdx];
    MeshComp->SetCollisionEnabled(NewMode);

#if WITH_EDITOR
    const FString ModeStr = UEnum::GetValueAsString(NewMode);

    UE_LOG(LogASPlacement, Log, TEXT("%s → %s"), *GetActorLabel(), *ModeStr);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetUniqueID()), 1.5f, FColor::Cyan,
            FString::Printf(TEXT("[%s] Collision: %s"), *GetActorLabel(), *ModeStr));
    }

    DrawDebugString(GetWorld(), GetActorLocation() + FVector(0,0,50.f), ModeStr, nullptr, FColor::Yellow, 1.0f, false);
#endif
}
