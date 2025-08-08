#include "ASPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ArcaneSouls/Systems/GridPuzzle/GridPuzzleManagerComponent.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include "ArcaneSouls/Systems/Combat/Data/ParryTypes.h"
#include "ArcaneSouls/Systems/Interfaces/DamageableInterface.h"
#include "ArcaneSouls/Systems/SpellSystem/Component/ASSpellComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ArcaneSouls/Systems/SpellSystem/Projectiles/ASProjectileBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogAS_Magic, Log, All);

AASPlayerCharacter::AASPlayerCharacter()
{
    if (CameraBoom)
    {
        CameraBoom->TargetArmLength = 350.f;
        CameraBoom->bUsePawnControlRotation = true;
    }

    if (FollowCamera)
    {
        FollowCamera->bUsePawnControlRotation = false;
    }
    SpellComp = CreateDefaultSubobject<UASSpellComponent>(TEXT("SpellComp"));
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
    bUseControllerRotationYaw = false;

    GridMgr = CreateDefaultSubobject<UGridPuzzleManagerComponent>(TEXT("GridMgr"));
}

void AASPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    GridMgr = FindComponentByClass<UGridPuzzleManagerComponent>();
    ensureMsgf(GridMgr, TEXT("[Puzzle] GridMgr NOT FOUND!"));
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* SubSys =
                LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                SubSys->AddMappingContext(IMC_Player, 0);
            }
        }
    }
}

void AASPlayerCharacter::SetupPlayerInputComponent(UInputComponent* IC)
{
    Super::SetupPlayerInputComponent(IC);
    UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(IC);

    EIC->BindAction(IA_Move_Forward , ETriggerEvent::Triggered, this, &AASPlayerCharacter::MoveForwardAxis);
    EIC->BindAction(IA_Move_Backward, ETriggerEvent::Triggered, this, &AASPlayerCharacter::MoveBackwardAxis);
    EIC->BindAction(IA_Move_Left    , ETriggerEvent::Triggered, this, &AASPlayerCharacter::MoveLeftAxis);
    EIC->BindAction(IA_Move_Right   , ETriggerEvent::Triggered, this, &AASPlayerCharacter::MoveRightAxis);
    EIC->BindAction(IA_Turn ,        ETriggerEvent::Triggered, this, &AASPlayerCharacter::TurnAxis);
    EIC->BindAction(IA_LookUp,       ETriggerEvent::Triggered, this, &AASPlayerCharacter::LookUpAxis);

    EIC->BindAction(IA_Jump,  ETriggerEvent::Started,   this, &ACharacter::Jump);
    EIC->BindAction(IA_Jump,  ETriggerEvent::Completed, this, &ACharacter::StopJumping);

    EIC->BindAction(IA_Attack , ETriggerEvent::Started, this, &AASPlayerCharacter::OnAttack);
    EIC->BindAction(IA_Dodge  , ETriggerEvent::Started, this, &AASPlayerCharacter::OnDodge);
    EIC->BindAction(IA_Guard  , ETriggerEvent::Started, this, &AASPlayerCharacter::TryParry);
    EIC->BindAction(IA_Guard  , ETriggerEvent::Completed, this, &AASPlayerCharacter::OnGuardEnd);
    EIC->BindAction(IA_Lockon , ETriggerEvent::Started, this, &AASPlayerCharacter::OnLockOn);
    EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &AASPlayerCharacter::OnInteract);
    EIC->BindAction(IA_Inventory,ETriggerEvent::Started, this, &AASPlayerCharacter::OnInventory);
    EIC->BindAction(IA_ESC   , ETriggerEvent::Started, this, &AASPlayerCharacter::OnPauseESC);

    // ─── Magic Charge 바인딩 ───────────────────────────────────────
    // 누르면 캐스팅 시작, 떼면 발사, 취소 버튼으로 전부 취소
    EIC->BindAction(IA_CastMagic,     ETriggerEvent::Started,   SpellComp, &UASSpellComponent::StartCharge);
    EIC->BindAction(IA_CastMagic,     ETriggerEvent::Completed, SpellComp, &UASSpellComponent::ReleaseCharge);
    EIC->BindAction(IA_CancelCast,    ETriggerEvent::Started,   SpellComp, &UASSpellComponent::CancelCharge);

    // 슬롯 선택
    EIC->BindAction(IA_SelectSpell1, ETriggerEvent::Started, this, &AASPlayerCharacter::SelectSpell1);
    EIC->BindAction(IA_SelectSpell2, ETriggerEvent::Started, this, &AASPlayerCharacter::SelectSpell2);
    EIC->BindAction(IA_SelectSpell3, ETriggerEvent::Started, this, &AASPlayerCharacter::SelectSpell3);
    EIC->BindAction(IA_SelectSpell4, ETriggerEvent::Started, this, &AASPlayerCharacter::SelectSpell4);
    EIC->BindAction(IA_SelectSpell5, ETriggerEvent::Started, this, &AASPlayerCharacter::SelectSpell5);
}

void AASPlayerCharacter::MoveForwardAxis(const FInputActionValue& Value)
{
    const float Axis = FMath::Clamp(Value.Get<float>(), -1.f, 1.f);
    if (FMath::IsNearlyZero(Axis)) return;
    const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    AddMovementInput(Dir, Axis);
}

void AASPlayerCharacter::MoveBackwardAxis(const FInputActionValue& Value)
{
    const float Axis = FMath::Clamp(Value.Get<float>(), -1.f, 1.f);
    if (FMath::IsNearlyZero(Axis)) return;
    const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    AddMovementInput(Dir, -Axis);
}

void AASPlayerCharacter::MoveRightAxis(const FInputActionValue& Value)
{
    const float Axis = FMath::Clamp(Value.Get<float>(), -1.f, 1.f);
    if (FMath::IsNearlyZero(Axis)) return;
    const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
    AddMovementInput(Dir, Axis);
}

void AASPlayerCharacter::MoveLeftAxis(const FInputActionValue& Value)
{
    const float Axis = FMath::Clamp(Value.Get<float>(), -1.f, 1.f);
    if (FMath::IsNearlyZero(Axis)) return;
    const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
    AddMovementInput(Dir, -Axis);
}

void AASPlayerCharacter::LookUpAxis(const FInputActionValue& Value)
{
    AddControllerPitchInput(-Value.Get<float>());
}

void AASPlayerCharacter::TurnAxis(const FInputActionValue& Value)
{
    AddControllerYawInput(Value.Get<float>());
}

FIntPoint AASPlayerCharacter::GetFacingDir4() const
{
    const FVector Fwd = Controller ? Controller->GetControlRotation().Vector() : GetActorForwardVector();
    return (FMath::Abs(Fwd.X) > FMath::Abs(Fwd.Y))
        ? (Fwd.X >= 0 ? FIntPoint{+1,0} : FIntPoint{-1,0})
        : (Fwd.Y >= 0 ? FIntPoint{0,+1}  : FIntPoint{0,-1});
}

void AASPlayerCharacter::CastFire()
{
    FVector CameraLocation;
    FRotator CameraRotation;
    GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector Start = CameraLocation;
    FVector End = Start + CameraRotation.Vector() * 2000.f;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && HitActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
        {
            IDamageableInterface::Execute_ApplyGridDamage(HitActor, 1);
        }
    }

#if !(UE_BUILD_SHIPPING)
    DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f, 0, 2.f);
#endif
}

void AASPlayerCharacter::CastIce()
{
    if (!FollowCamera) return;
    const FVector Start = FollowCamera->GetComponentLocation();
    const FVector Dir = FollowCamera->GetForwardVector();
    const FVector End = Start + Dir * 4000.f;

    FHitResult Hit;
    FCollisionQueryParams Params(NAME_None, false, this);
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, Start, End, ECC_Visibility, Params); // <-- 채널은 데미지/불과 동일하게

    DrawDebugLine(GetWorld(), Start, End,
        bHit ? FColor::Cyan : FColor::Blue, false, 2.f, 0, 2.f);

    if (bHit)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && HitActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
        {
            // 바닥 or 벽 모두 ICE 호출 (ApplyGridIce_Implementation 실행)
            IDamageableInterface::Execute_ApplyGridIce(HitActor, 1);
        }
    }
}

void AASPlayerCharacter::OnAttack()
{
}
void AASPlayerCharacter::OnDodge    () {}
void AASPlayerCharacter::OnGuardStart()
{
    UE_LOG(LogTemp, Warning, TEXT("🛡️ Guard Started"));

    // 임시 이펙트나 사운드 트리거

}

void AASPlayerCharacter::OnGuardEnd()
{
    UE_LOG(LogTemp, Warning, TEXT("🛑 Guard Ended"));


    // 이펙트 제거 또는 상태 초기화
}
void AASPlayerCharacter::OnLockOn   () {}
void AASPlayerCharacter::OnInteract () {}
void AASPlayerCharacter::OnInventory() {}
void AASPlayerCharacter::OnPauseESC () {}

void AASPlayerCharacter::SelectSpell1(const FInputActionValue& Value) { SpellComp->SetActiveSpellIndex(0); }
void AASPlayerCharacter::SelectSpell2(const FInputActionValue& Value) { SpellComp->SetActiveSpellIndex(1); }
void AASPlayerCharacter::SelectSpell3(const FInputActionValue& Value) { SpellComp->SetActiveSpellIndex(2); }
void AASPlayerCharacter::SelectSpell4(const FInputActionValue& Value) { SpellComp->SetActiveSpellIndex(3); }
void AASPlayerCharacter::SelectSpell5(const FInputActionValue& Value) { SpellComp->SetActiveSpellIndex(4); }

// --- GetMagicPower 정의 ---
float AASPlayerCharacter::GetMagicPower() const
{
    // 컴포넌트 쪽에 MagicPower가 있으면 그걸 반환하거나,
    // 만약 ASPlayerCharacter 자체에 MagicPower 프로퍼티가 있었다면 그대로 반환
    if (SpellComp)
        return SpellComp->GetMagicPower();  
    return 1.0f;
}

// --- MP 접근자 정의 ---
float AASPlayerCharacter::GetCurrentMP() const
{
    return CurrentMP;  // CurrentMP는 헤더에서 UPROPERTY로 관리된 값이어야 합니다
}

void AASPlayerCharacter::ModifyMP(float Delta)
{
    // MP 최소 0, 최대 MaxMP 범위로 클램프
    CurrentMP = FMath::Clamp(CurrentMP + Delta, 0.0f, MaxMP);
}
