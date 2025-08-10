// ======================================================================
//  SerynethAIController.h – Arcane Souls 보스: 세리네스 AI 컨트롤러
// ======================================================================

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SerynethAIController.generated.h"

UCLASS()
class ARCANESOULS_API ASerynethAIController : public AAIController
{
	GENERATED_BODY()

public: // UFUNCTION (public)
    ASerynethAIController();

protected: // UFUNCTION (protected)
    virtual void BeginPlay() override;

    /** 초기 블랙보드 / 트리 설정 */
    void StartAI();

protected: // UPROPERTY (protected)
    UPROPERTY(EditDefaultsOnly, Category="AI")
    UBehaviorTree* BehaviorTreeAsset;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    UBlackboardData* BlackboardAsset;

    virtual void OnPossess(APawn* InPawn) override;
};
