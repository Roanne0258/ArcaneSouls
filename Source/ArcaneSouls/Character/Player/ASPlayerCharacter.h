// ======================================================================
//  ASPlayerCharacter.h   –   Arcane Souls 기본 플레이어 캐릭터
// ======================================================================
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ASPlayerCharacter.generated.h"

/* ───── Forward Decls ───── */
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UGridPuzzleManagerComponent;
struct FInputActionValue;

/* ────────────────────────────────────────────────────────────── */
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
    /* ───── ACharacter overrides ───── */
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    /* ───── Components ───── */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    USpringArmComponent* CameraBoom = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    UCameraComponent* FollowCamera = nullptr;

    /* ───── Enhanced-Input: Mapping Context ───── */
    UPROPERTY(EditDefaultsOnly, Category="Input|IMC")
    UInputMappingContext* IMC_Player = nullptr;

    /* ───── Enhanced-Input: Action Assets ───── */
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_Move_Forward  = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_Move_Backward = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_Move_Left     = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_Move_Right    = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_Turn          = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_LookUp        = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_Jump          = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_Dodge         = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_Guard         = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_Attack        = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_Lockon        = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_Interact      = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_Inventory     = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_ESC           = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_CastFire      = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") UInputAction* IA_CastIce       = nullptr;

    /* ───── Enhanced-Input: Callback Handlers ───── */
    void MoveForwardAxis(const FInputActionValue& Value);
    void MoveRightAxis  (const FInputActionValue& Value);
    void MoveBackwardAxis(const FInputActionValue& Value);
    void MoveLeftAxis     (const FInputActionValue& Value);
    void TurnAxis       (const FInputActionValue& Value);
    void LookUpAxis     (const FInputActionValue& Value);

    void OnAttack   ();
    void OnDodge    ();
    void OnGuardStart();
    void OnGuardEnd ();
    void OnLockOn   ();
    void OnInteract ();
    void OnInventory();
    void OnPauseESC ();

    /* ───── Grid Puzzle Helpers ───── */
    UPROPERTY() UGridPuzzleManagerComponent* GridMgr = nullptr;
    FIntPoint  GetFacingDir4() const;
};
