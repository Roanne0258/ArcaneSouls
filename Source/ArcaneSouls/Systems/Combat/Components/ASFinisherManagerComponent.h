// ======================================================================
//  ASFinisherManagerComponent.h   –   Arcane Souls 피니셔 컷씬 매니저
// ======================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ASFinisherManagerComponent.generated.h"

/* ───── Forward Decls ───── */
class UAnimMontage;
class AASCharacterBase;

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class ARCANESOULS_API UASFinisherManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Variables

public:
	// Functions
	UASFinisherManagerComponent();

	/** 피니셔 컷씬 재생 시작 */
	void PlayFinisher(AActor* Target, AActor* Instigator);

protected:
	// Variables
	UPROPERTY(EditDefaultsOnly, Category="Finisher")
	UAnimMontage* FinisherMontage;

	UPROPERTY()
	AASCharacterBase* OwnerCharacter;

	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	FTimerHandle FinisherTimer;

    // [Tuning] 파라미터화된 시간/연출
    UPROPERTY(EditDefaultsOnly, Category="Finisher|Time")
    float GlobalTimeDilation = 0.3f;

    UPROPERTY(EditDefaultsOnly, Category="Finisher|Time")
    float PlayerTimeDilation = 0.05f;

    UPROPERTY(EditDefaultsOnly, Category="Finisher|Timing")
    float MontageDelaySec = 0.15f;

    UPROPERTY(EditDefaultsOnly, Category="Finisher|Timing")
    float CutsceneLengthSec = 1.95f;

protected:
	// Functions
	virtual void BeginPlay() override;

	/** 컷씬 단계별 연출 처리 */
	void StartTimeDilation();
	void PlayMontage();
	void EndFinisher();
};
