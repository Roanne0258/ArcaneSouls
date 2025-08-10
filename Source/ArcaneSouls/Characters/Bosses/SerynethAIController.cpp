#include "SerynethAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ArcaneSouls/Core/ASLogChannels.h"

ASerynethAIController::ASerynethAIController()
{
	bWantsPlayerState = false;
}

void ASerynethAIController::BeginPlay()
{
	Super::BeginPlay();

	UBlackboardComponent* RawBBComp = Blackboard.Get();
	if (UseBlackboard(BlackboardAsset, RawBBComp))
	{
		RunBehaviorTree(BehaviorTreeAsset);

        // [AI] 플레이어 타겟 세팅
		AActor* Player = UGameplayStatics::GetPlayerPawn(this, 0);
		if (Player)
		{
			Blackboard->SetValueAsObject("TargetActor", Player);
            UE_LOG(LogAS_AI, Display, TEXT("TargetActor = %s"), *Player->GetName());
		}
	}
}

void ASerynethAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
}
