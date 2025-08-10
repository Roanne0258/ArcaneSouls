#include "AnimNotifyState_AttackTrace.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "ArcaneSouls/Characters/ASCharacterBase.h"
#include "ArcaneSouls/Core/ASLogChannels.h"

void UAnimNotifyState_AttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp) return;
	PrevTip = MeshComp->GetSocketLocation(TipSocket);
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
			if (AASCharacterBase* Player = Cast<AASCharacterBase>(Hit.GetActor()))
			{
				// 중복 방지 필요시 처리 추가
				// Player->OnHitByBoss(); // 임시 처리 함수 (직접 구현 필요)
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
	// 종료 시 별도 처리 필요 없다면 생략 가능
}
