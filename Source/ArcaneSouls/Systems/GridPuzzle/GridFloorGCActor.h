// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridCellBaseActor.h"
#include "GridFloorGCActor.generated.h"

/**
 * 
 */
UCLASS()
class ARCANESOULS_API AGridFloorGCActor : public AGridCellBaseActor
{
	GENERATED_BODY()
public:
	AGridFloorGCActor();
	void ApplyGridDamage_Implementation(int32 Amount);
	void ApplyGridIce_Implementation(int32 Amount);

protected:
	UPROPERTY(VisibleAnywhere)
	class UGeometryCollectionComponent* GCComp;
};
