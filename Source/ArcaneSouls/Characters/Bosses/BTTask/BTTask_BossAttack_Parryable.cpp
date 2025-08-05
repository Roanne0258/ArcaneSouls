// ======================================================================
//  BTTask_BossAttack_Parryable.cpp – Arcane Souls 보스 공격 태스크 (패링 대응)
// ======================================================================

#include "BTTask_BossAttack_Parryable.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"

UBTTask_BossAttack_Parryable::UBTTask_BossAttack_Parryable()
{
	NodeName = "Boss Attack (Parryable)";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_BossAttack_Parryable::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	ACharacter* BossChar = AICon ? Cast<ACharacter>(AICon->GetPawn()) : nullptr;
	if (!BossChar || !AttackMontage) return EBTNodeResult::Failed;

	UAnimInstance* AnimInst = BossChar->GetMesh()->GetAnimInstance();
	if (!AnimInst) return EBTNodeResult::Failed;

	if (AnimInst->Montage_IsPlaying(AttackMontage))
	{
		AnimInst->Montage_Stop(0.1f);
	}

	AnimInst->Montage_Play(AttackMontage, PlayRate);
	bIsPlaying = true;

	return EBTNodeResult::InProgress;
}

void UBTTask_BossAttack_Parryable::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	ACharacter* BossChar = AICon ? Cast<ACharacter>(AICon->GetPawn()) : nullptr;
	if (!BossChar || !AttackMontage) return;

	UAnimInstance* AnimInst = BossChar->GetMesh()->GetAnimInstance();
	if (!AnimInst || !bIsPlaying) return;

	if (!AnimInst->Montage_IsPlaying(AttackMontage))
	{
		bIsPlaying = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
