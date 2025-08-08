#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ASSpellComponent.generated.h"

class AASProjectileBase;

USTRUCT(BlueprintType)
struct FSpellData : public FTableRowBase
{
	GENERATED_BODY()

	/** 프로젝타일 클래식 */
	UPROPERTY(EditAnywhere) TSubclassOf<AASProjectileBase> ProjectileClass;

	/** 소켓 이름 */
	UPROPERTY(EditAnywhere) FName HandSocketName;

	/** 초기 속도 */
	UPROPERTY(EditAnywhere) float ProjectileSpeed = 2500.f;

	/** 기본 데미지 */
	UPROPERTY(EditAnywhere) float BaseDamage = 10.f;

	/** 스케일 계수 */
	UPROPERTY(EditAnywhere) float DamageScale = 1.0f;
};

// 로그 카테고리 선언
DECLARE_LOG_CATEGORY_EXTERN(LogAS_Spell, Log, All);

UCLASS( ClassGroup=(Magic), meta=(BlueprintSpawnableComponent) )
class ARCANESOULS_API UASSpellComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UASSpellComponent();

	virtual void BeginPlay() override;
	// 초기화 함수 (나중에 DataTable 로딩도 여기서 분기)
	void InitializeSpell();

	// 즉시 발사 API
	void CastInstant();

	// 캐스팅(Charge) API
	void StartCharge();
	void ReleaseCharge();
	void CancelCharge();

	/** 현재 마법력 반환 */
	UFUNCTION(BlueprintCallable, Category="Magic")
	float GetMagicPower() const { return MagicPower; }
	
	// --- 슬롯 선택 API ---
	void SetActiveSpellIndex(int32 NewIndex);

protected:
	// Balance & Data
	UPROPERTY(EditAnywhere, Category="Magic|Data")
	UDataTable*    SpellDataTable;             // (나중에 DataTable 연동 시 사용)

	UPROPERTY(EditAnywhere, Category="Magic|Data")
	TArray<FName>  SpellRowNames;              // 1~5 슬롯에 대응하는 RowNames

private:
	// 현재 활성 슬롯
	int32 ActiveSpellIndex = 0;

	// 슬롯 데이터 로드
	void LoadSpellData(const FName& RowName);
	
	// ─── Balance & Data ─────────────────────────────────────
	UPROPERTY(EditAnywhere, Category="Magic|Stats")
	float MagicPower = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Magic|Projectile")
	TSubclassOf<class AASProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category="Magic|Projectile")
	FName HandSocketName = TEXT("Hand_R");

	UPROPERTY(EditDefaultsOnly, Category="Magic|Projectile", meta=(ClampMin="0.0"))
	float ProjectileSpeed = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category="Magic|Projectile", meta=(ClampMin="0.0"))
	float BaseDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Magic|Projectile", meta=(ClampMin="0.0"))
	float DamageScale = 1.0f;

	// ─── Charge System ──────────────────────────────────────
	UPROPERTY(EditAnywhere, Category="Magic|Charge")
	float ChargeInterval = 0.5f;

	UPROPERTY(EditAnywhere, Category="Magic|Charge")
	int32 MaxChargeStacks = 5;

	UPROPERTY(EditAnywhere, Category="Magic|Charge")
	float MPPerStack = 10.f;

	UPROPERTY(EditAnywhere, Category="Magic|Charge")
	TSubclassOf<AActor> ChargeOrbClass;

	// Runtime state
	bool bIsCharging = false;
	int32 CurrentStacks = 0;
	FTimerHandle ChargeTimerHandle;

	UPROPERTY()
	TArray<AActor*> ChargeOrbs;

	// 내부 콜백
	void HandleChargeStack();
	
};
