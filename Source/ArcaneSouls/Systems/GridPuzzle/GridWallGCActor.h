// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridCellBaseActor.h"
#include "GridWallGCActor.generated.h"

class UGridPuzzleManagerComponent;
/**
 * 
 */
UCLASS()
class ARCANESOULS_API AGridWallGCActor : public AGridCellBaseActor
{
	GENERATED_BODY()
public:
	AGridWallGCActor();
	void BeginPlay();
	void ApplyGridDamage_Implementation(int32 Amount);
	void ApplyGridIce_Implementation(int32 Amount);
	UPROPERTY()
	UGridPuzzleManagerComponent* GridMgr = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FIntPoint GridA;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FIntPoint GridB;

protected:
	UPROPERTY(VisibleAnywhere)
	class UGeometryCollectionComponent* GCComp;
};
