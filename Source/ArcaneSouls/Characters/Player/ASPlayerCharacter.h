// ======================================================================
//  ASPlayerCharacter.h   –   Arcane Souls 기본 플레이어 캐릭터
// ======================================================================
#pragma once

#include "CoreMinimal.h"
#include "ArcaneSouls/Characters/ASCharacterBase.h"
#include "GameFramework/Character.h"
#include "ASPlayerCharacter.generated.h"

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

    UFUNCTION()
    void CastMagic();
protected:
    /* ───── ACharacter overrides ───── */
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    /** 매직 스탯 (예: Intelligence 기반) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float MagicPower = 1.0f;
    // ─── Magic Settings ───────────────────────────────────────

    /** 프로젝타일 스폰 클래스 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Magic")
    TSubclassOf<AASProjectileBase> MagicProjectileClass;

    /** 발사 지점으로 사용할 메쉬 소켓 이름 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Magic")
    FName HandSocketName = TEXT("Hand_R");

    /** 프로젝타일 초기 속도 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Magic", meta=(ClampMin="0.0"))
    float MagicProjectileSpeed = 2500.f;

    /** 프로젝타일 기본 데미지 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Magic", meta=(ClampMin="0.0"))
    float MagicBaseDamage = 10.f;

    /** 플레이어 스탯(MagicPower) 곱연산 계수 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Magic", meta=(ClampMin="0.0"))
    float MagicDamageScale = 1.0f;
    
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
    UPROPERTY()
    UGridPuzzleManagerComponent* GridMgr = nullptr;

    FIntPoint GetFacingDir4() const;
};
