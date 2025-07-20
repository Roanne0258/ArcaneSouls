#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ASPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGridPuzzleManagerComponent;

UCLASS(Blueprintable)
class ARCANESOULS_API AASPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AASPlayerCharacter();

	/* ───── Gameplay Actions ───── */
	UFUNCTION(BlueprintCallable, Category="Puzzle|Magic") void CastFire();
	UFUNCTION(BlueprintCallable, Category="Puzzle|Magic") void CastIce();

protected:
	/* ───── ACharacter overrides ─ */
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/* ───── Components ───── */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera") USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera") UCameraComponent*   FollowCamera;

	/* ───── Movement helpers ─ */
	void MoveForward(float Value);
	void MoveRight (float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	/* ───── Internal ───── */
	UPROPERTY() UGridPuzzleManagerComponent* GridMgr = nullptr;
	FIntPoint   GetFacingDir4() const;

	static constexpr float TurnRate  = 45.f;  // deg/sec
	static constexpr float LookRate  = 45.f;
};