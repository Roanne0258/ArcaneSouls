#include "AnimNotifyState_AttackTrace.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "ArcaneSouls/Characters/ASCharacterBase.h"
#include "ArcaneSouls/Core/ASLogChannels.h"

void UAnimNotifyState_AttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp) return;
	PrevTip = MeshComp->GetSocketLocation(TipSocket);
    HitActorsThisFrame.Reset();
}

void UAnimNotifyState_AttackTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	const FVector CurrentTip = MeshComp->GetSocketLocation(TipSocket);

	FCollisionShape Shape = FCollisionShape::MakeSphere(TraceRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	TArray<FHitResult> Hits;
    if (MeshComp->GetWorld()->SweepMultiByChannel(Hits, PrevTip, CurrentTip, FQuat::Identity, ECC_Pawn, Shape, Params))
	{
		for (const FHitResult& Hit : Hits)
		{
            AActor* Victim = Hit.GetActor();
            if (!Victim)
			{
                continue;
            }

            if (HitActorsThisFrame.Contains(Victim))
            {
                continue;
            }
            HitActorsThisFrame.Add(Victim);

            if (AASCharacterBase* Character = Cast<AASCharacterBase>(Victim))
            {
                // 간단 분기: 가드 중이면 블록 처리, 아니면 후속 처리(데미지/경직 등)로 위임
                Character->OnGuardBlockHit(/*Damage*/1.f, Owner);
			}
		}
	}

#if WITH_EDITOR
	DrawDebugLine(MeshComp->GetWorld(), PrevTip, CurrentTip, FColor::Red, false, 1.f, 0, 2.f);
	DrawDebugSphere(MeshComp->GetWorld(), CurrentTip, TraceRadius, 12, FColor::Orange, false, 1.f);
#endif

	PrevTip = CurrentTip;
}

void UAnimNotifyState_AttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    HitActorsThisFrame.Reset();
}
