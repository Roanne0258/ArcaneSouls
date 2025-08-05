#include "SerynethAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

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

		// 플레이어 타겟 세팅 (아까 설명대로)
		AActor* Player = UGameplayStatics::GetPlayerPawn(this, 0);
		if (Player)
		{
			Blackboard->SetValueAsObject("TargetActor", Player);
			UE_LOG(LogTemp, Warning, TEXT("OnPossess"));
		}
	}
}

void ASerynethAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
}
