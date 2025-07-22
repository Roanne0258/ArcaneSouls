// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridCellBaseActor.h"
#include "GridWallGCActor.generated.h"

/**
 * 
 */
UCLASS()
class ARCANESOULS_API AGridWallGCActor : public AGridCellBaseActor
{
	GENERATED_BODY()
public:
	AGridWallGCActor();
	void ApplyGridDamage_Implementation(int32 Amount);

protected:
	UPROPERTY(VisibleAnywhere)
	class UGeometryCollectionComponent* GCComp;
};
