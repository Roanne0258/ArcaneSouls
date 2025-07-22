#pragma once

#include "CoreMinimal.h"
#include "ArcaneSouls/Editor/Placement/PlacementHelperActor.h"
#include "ArcaneSouls/Systems/Interfaces/DamageableInterface.h"
#include "GameFramework/Actor.h"
#include "GridCellBaseActor.generated.h"

/** 바닥·벽 공통 부모 */
UCLASS(Abstract)
class ARCANESOULS_API AGridCellBaseActor : public APlacementHelperActor, public IDamageableInterface
{
	GENERATED_BODY()

public:

	AGridCellBaseActor();
	/** 에디터에서 직접 조정 가능한 체력 (0~3) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	int32 Health = 3;

	/** Health 텍스트 업데이트 */
	void RefreshVisual();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grid")
	void ApplyGridDamage(int32 Amount);
protected:
	virtual void BeginPlay() override;
	/** 시각 - Mesh (BP에서 StaticMesh 지정) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh = nullptr;

	/** 머리 위 체력 숫자 표시 */
	UPROPERTY(VisibleAnywhere)
	class UTextRenderComponent* HealthText;


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;
	void ApplyGridDamage_Implementation(int32 Amount);
#endif
};
