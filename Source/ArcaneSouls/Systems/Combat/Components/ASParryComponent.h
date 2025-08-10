// ======================================================================
//  ASParryComponent.h   –   Arcane Souls 퍼펙트 패링 판정 컴포넌트
// ======================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArcaneSouls/Systems/Combat/Data/ParryTypes.h"
#include "ASParryComponent.generated.h"

/* ───── Forward Decls ───── */
class AASCharacterBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARCANESOULS_API UASParryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Variables

public:
	// Functions
	UASParryComponent();

	/** 현재 등록된 패링 윈도우에 대해 판정 수행 */
	void EvaluateParry();

	/** 공격자 측에서 현재 패링 윈도우 등록 */
	void SetParryWindow(const FParryWindow& InWindow);

	/** 패링 윈도우 초기화 (NotifyEnd 등에서 호출) */
	void ClearParryWindow();

protected:
	// Variables
	UPROPERTY()
	AASCharacterBase* OwnerCharacter;

	UPROPERTY()
	FParryWindow CurrentParryWindow;

	UPROPERTY()
	bool bParryWindowActive = false;

protected:
	// Functions
	virtual void BeginPlay() override;

	/** 패링 성공 처리 → 태그 부여 + Finisher */
	void HandleParrySuccess();

	/** 패링 실패 → Guard 가능 여부 판별 */
	void HandleParryFail();

private:

	// 윈도우 종료 예약용 타이머
	FTimerHandle ParryWindowTimerHandle;

	// 노티파이 End가 너무 빨라도, "윈도우 종료시각"에 맞춰 지우도록 예약
	void ScheduleParryWindowClear(float EndTimeSeconds);

	// 타이머 콜백
	void OnParryWindowTimerExpired();
};
