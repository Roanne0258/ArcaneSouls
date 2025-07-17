#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlacementHelperActor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogASPlacement, Log, All);

UCLASS()
class ARCANESOULS_API APlacementHelperActor : public AActor
{
	GENERATED_BODY()

	APlacementHelperActor();
	/* ──────────────── Public Vars ──────────────── */
public:
	/** 배치 대상 메시 (Instance Editable) */
	UPROPERTY(EditInstanceOnly, Category = "Placement")
	UStaticMeshComponent* MeshComp = nullptr;

	/** 이동·변경 시 자동 정렬 여부 */               // NEW
	UPROPERTY(EditAnywhere, Category = "Placement")
	bool bAutoAlign = true;

	/** 정렬 기준: true = 메시 하단, false = 피벗 */ // NEW
	UPROPERTY(EditAnywhere, Category = "Placement")
	bool bUseMeshBottom = true;

	/* ──────────────── Public Funcs ─────────────── */
public:
	/** 버튼 수동 호출용 */
	UFUNCTION(CallInEditor, Category = "Placement")
	void AlignToGround();

	/* ──────────────── Protected ─────────────── */
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
};
