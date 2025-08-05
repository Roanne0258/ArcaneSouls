#pragma once

#include "CoreMinimal.h"
#include "ArcaneSouls/Editor/Placement/PlacementHelperActor.h"
#include "ArcaneSouls/Systems/Interfaces/DamageableInterface.h"
#include "GameFramework/Actor.h"
#include "GridCellBaseActor.generated.h"

/**
 * 바닥·벽 공통 부모 액터
 */
UCLASS(Abstract)
class ARCANESOULS_API AGridCellBaseActor 
	: public APlacementHelperActor, public IDamageableInterface
{
	GENERATED_BODY()

public: // ▼ Public Variables -----------------------------

	/** 에디터에서 직접 조정 가능한 체력 (0~3) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 Health = 3;

	/** 그리드 좌표 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FIntPoint GridCoord = FIntPoint::ZeroValue;

public: // ▼ Public Functions -----------------------------

	AGridCellBaseActor();

	/** Health 텍스트를 시각적으로 갱신 */
	void RefreshVisual();

	/** 그리드 데미지 적용 (Blueprint 확장 가능) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grid")
	void ApplyGridDamage(int32 Amount);

protected: // ▼ Protected Variables ------------------------

	/** 시각용 Static Mesh (BP에서 설정) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh = nullptr;

	/** 체력을 표시하는 텍스트 렌더 컴포넌트 */
	UPROPERTY(VisibleAnywhere)
	class UTextRenderComponent* HealthText;

protected: // ▼ Protected Functions -------------------------

	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;
	void ApplyGridDamage_Implementation(int32 Amount);
#endif

};
