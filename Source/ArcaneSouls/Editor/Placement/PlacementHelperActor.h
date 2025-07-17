#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlacementHelperActor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogASPlacement, Log, All);

UCLASS()
class ARCANESOULS_API APlacementHelperActor : public AActor
{
	GENERATED_BODY()
	
	/* ──────────────── Public Vars ──────────────── */
public:
	/** 정렬 대상. 비워두면 RootComponent 사용 */
	UPROPERTY(EditAnywhere, Category = "Placement")
	USceneComponent* TargetComponent = nullptr;

	/* ──────────────── Public Funcs ─────────────── */
public:
	/** 바닥으로 Z 위치 맞추기 */
	UFUNCTION(CallInEditor, Category = "Placement")
	void AlignToGround();

	/* ─────────────── Protected / Private ───────── */
};
