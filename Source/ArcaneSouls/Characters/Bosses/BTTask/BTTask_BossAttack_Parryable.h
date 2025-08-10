// ======================================================================
//  BTTask_BossAttack_Parryable.h – Arcane Souls 보스 공격 태스크 (패링 대응)
// ======================================================================

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossAttack_Parryable.generated.h"

UCLASS()
class ARCANESOULS_API UBTTask_BossAttack_Parryable : public UBTTaskNode
{
	GENERATED_BODY()

public: // UFUNCTION (public)
    UBTTask_BossAttack_Parryable();

protected: // UPROPERTY (protected)
    /** 공격 몽타주 */
    UPROPERTY(EditAnywhere, Category="AI|Attack")
    UAnimMontage* AttackMontage;

    /** 몽타주 재생 속도 */
    UPROPERTY(EditAnywhere, Category="AI|Attack")
    float PlayRate = 1.0f;

protected: // UFUNCTION (protected)
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    /** 종료를 기다릴 경우 필요 */
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    bool bIsPlaying = false;
};
