// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ASPlayerController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class ARCANESOULS_API AASPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AASPlayerController();

protected:
	/* BeginPlay 시 IMC 삽입 */
	virtual void BeginPlay() override;

	/* ESC 키 바인딩 (EnhancedInput) */
	virtual void SetupInputComponent() override;

	// ───────── UI / Pause ─────────
	/** ESC 입력 시 BP 에서 메뉴 토글 구현 */
	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void TogglePauseMenu();

	// ───────── IMC / IA 애셋 ─────────
	/** 플레이어 공통 MappingContext */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* IMC_Player = nullptr;

	/** ESC 액션 (IMC 에도 매핑해야 함) */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_ESC = nullptr;

private:
	/* ESC 콜백 */
	void HandlePause(const FInputActionValue& /*Value*/);
};
