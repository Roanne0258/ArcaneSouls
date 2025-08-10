// ======================================================================
//  ASPlayerCharacter.h   –   Arcane Souls 기본 플레이어 캐릭터
// ======================================================================
#pragma once

#include "CoreMinimal.h"
#include "ArcaneSouls/Characters/ASCharacterBase.h"
#include "GameFramework/Character.h"
#include "ASPlayerCharacter.generated.h"

class UASSpellComponent;
class AASProjectileBase;
/* ───── Forward Decls ───── */
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UGridPuzzleManagerComponent;
struct FInputActionValue;

/* ────────────────────────────────────────────────────────────── */
UCLASS(Blueprintable)
class ARCANESOULS_API AASPlayerCharacter : public AASCharacterBase
{
    GENERATED_BODY()

public:
    AASPlayerCharacter();
    
    /* ───── Gameplay Actions ───── */
    UFUNCTION(BlueprintCallable, Category="Puzzle|Magic") void CastFire();
    UFUNCTION(BlueprintCallable, Category="Puzzle|Magic") void CastIce();

    /** 현재 매직 파워 반환 */
    UFUNCTION(BlueprintCallable, Category="Stats")
    float GetMagicPower() const;
    
    // MP 관련 함수들
    float GetCurrentMP() const;
    void ModifyMP(float Delta);

    void OnGuardPressed(const FInputActionValue& Value);
    void OnGuardEnd(const FInputActionValue& Value);

protected:
    /* ───── ACharacter overrides ───── */
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UASSpellComponent* SpellComp;

    /* ───── Grid Puzzle Helpers ───── */
    UPROPERTY()
    UGridPuzzleManagerComponent* GridMgr = nullptr;
   
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
    void OnLockOn   ();
    void OnInteract ();
    void OnInventory();
    void OnPauseESC ();

    // ─── 슬롯 선택용 InputAction ─────────────────────
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") 
    UInputAction* IA_SelectSpell1 = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") 
    UInputAction* IA_SelectSpell2 = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") 
    UInputAction* IA_SelectSpell3 = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") 
    UInputAction* IA_SelectSpell4 = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") 
    UInputAction* IA_SelectSpell5 = nullptr;

    // ─── Cast·Cancel용 InputAction ───────────────────
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") 
    UInputAction* IA_CastMagic  = nullptr;
    UPROPERTY(EditDefaultsOnly, Category="Input|IA") 
    UInputAction* IA_CancelCast = nullptr;

    // 슬롯 선택 콜백
    void SelectSpell1(const FInputActionValue& Value);
    void SelectSpell2(const FInputActionValue& Value);
    void SelectSpell3(const FInputActionValue& Value);
    void SelectSpell4(const FInputActionValue& Value);
    void SelectSpell5(const FInputActionValue& Value);

    FIntPoint GetFacingDir4() const;
};
