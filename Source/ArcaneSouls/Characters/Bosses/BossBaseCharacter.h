// ======================================================================
//  BossBaseCharacter.h   –   Arcane Souls 보스 캐릭터 베이스 클래스
// ======================================================================

#pragma once

#include "CoreMinimal.h"
#include "ArcaneSouls/Characters/ASCharacterBase.h"
#include "ArcaneSouls/Systems/Combat/Data/ParryTypes.h"
#include "BossBaseCharacter.generated.h"

UCLASS()
class ARCANESOULS_API ABossBaseCharacter : public AASCharacterBase
{
	GENERATED_BODY()

public: // UPROPERTY (public)
    // 없음

public: // UFUNCTION (public)
	ABossBaseCharacter();

	/* ───── 전투 이벤트 ───── */
	virtual void ReceiveFinisher(AActor* FinisherSource) override;

    // HP 인터페이스 override
    virtual float GetMaxHP() const override;
    virtual void ApplyChipDamage(float Amount) override;

	/* ───── 속성 디버프 적용 ───── */
	void ApplyElementDebuff(EParryElementType Element);

	/* ───── 공격 판정 ───── */

protected: // UPROPERTY (protected)
    UPROPERTY(VisibleAnywhere, Category="Boss")
	bool bIsStunned = false;

	UPROPERTY(EditDefaultsOnly, Category="Boss")
	float MaxHP = 1000.f;

	UPROPERTY(VisibleAnywhere, Category="Boss")
	float CurrentHP = 1000.f;

	UPROPERTY(EditAnywhere, Category="Boss|Debuff")
	TMap<EParryElementType, float> DebuffDurations;

protected: // UFUNCTION (protected)
	virtual void BeginPlay() override;

	/** 보스 피니셔 컷씬 중 HP 감소 + 디버프 적용 */
	void OnFinisherHit();

    /** 속성별 디버프 로직 (감쇠 등은 효과 측에서 처리) */
    void ApplyDebuffEffect(EParryElementType Type);
};
