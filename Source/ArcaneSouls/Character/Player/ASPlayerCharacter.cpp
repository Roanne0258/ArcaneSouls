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

AASPlayerCharacter::AASPlayerCharacter()
{
    /* Capsule defaults come from ACharacter */

    // ─ Camera boom
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 350.f;
    CameraBoom->bUsePawnControlRotation = true;

    // ─ Follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // Character movement tweaks (optional)
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
    bUseControllerRotationYaw = false;
}

void AASPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    GridMgr = FindComponentByClass<UGridPuzzleManagerComponent>();
    /* IMC 등록 */
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* SubSys =
                LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                SubSys->AddMappingContext(IMC_Player, /*Priority=*/0);
            }
        }
    }
}

/* ───────────── Input Bindings ───────────── */
void AASPlayerCharacter::SetupPlayerInputComponent(UInputComponent* IC)
{
    Super::SetupPlayerInputComponent(IC);
    UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(IC);

    /* ─ Axis ─ */
    EIC->BindAction(IA_Move_Forward , ETriggerEvent::Triggered, this, &AASPlayerCharacter::MoveForwardAxis);
    EIC->BindAction(IA_Move_Backward, ETriggerEvent::Triggered, this, &AASPlayerCharacter::MoveBackwardAxis); // NEW
    EIC->BindAction(IA_Move_Left    , ETriggerEvent::Triggered, this, &AASPlayerCharacter::MoveLeftAxis);     // NEW
    EIC->BindAction(IA_Move_Right   , ETriggerEvent::Triggered, this, &AASPlayerCharacter::MoveRightAxis );
    EIC->BindAction(IA_Turn ,        ETriggerEvent::Triggered, this, &AASPlayerCharacter::TurnAxis );
    EIC->BindAction(IA_LookUp,       ETriggerEvent::Triggered, this, &AASPlayerCharacter::LookUpAxis);

    /* ─ Jump (Enhanced) ─ */
    EIC->BindAction(IA_Jump,  ETriggerEvent::Started,   this, &ACharacter::Jump);
    EIC->BindAction(IA_Jump,  ETriggerEvent::Completed, this, &ACharacter::StopJumping);

    /* ─ Combat / Misc ─ */
    EIC->BindAction(IA_Attack , ETriggerEvent::Started, this, &AASPlayerCharacter::OnAttack );
    EIC->BindAction(IA_Dodge  , ETriggerEvent::Started, this, &AASPlayerCharacter::OnDodge  );
    EIC->BindAction(IA_Guard  , ETriggerEvent::Started, this, &AASPlayerCharacter::OnGuardStart);
    EIC->BindAction(IA_Guard  , ETriggerEvent::Completed, this, &AASPlayerCharacter::OnGuardEnd);
    EIC->BindAction(IA_Lockon , ETriggerEvent::Started, this, &AASPlayerCharacter::OnLockOn );
    EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &AASPlayerCharacter::OnInteract );
    EIC->BindAction(IA_Inventory,ETriggerEvent::Started, this, &AASPlayerCharacter::OnInventory );
    EIC->BindAction(IA_ESC   , ETriggerEvent::Started, this, &AASPlayerCharacter::OnPauseESC );

    /* ─ 기존 Magic ─ */
    EIC->BindAction(IA_CastFire, ETriggerEvent::Started, this, &AASPlayerCharacter::CastFire);
    EIC->BindAction(IA_CastIce , ETriggerEvent::Started, this, &AASPlayerCharacter::CastIce );
}

/* ───────── Axis 구현 ───────── */
void AASPlayerCharacter::MoveForwardAxis(const FInputActionValue& Value)
{
    const float Axis =  FMath::Clamp(Value.Get<float>(), -1.f, 1.f);   // +1
    if (FMath::IsNearlyZero(Axis)) return;

    const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector  Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    AddMovementInput(Dir,  Axis);        // 앞으로
}
void AASPlayerCharacter::MoveBackwardAxis(const FInputActionValue& Value)
{
    const float Axis =  FMath::Clamp(Value.Get<float>(), -1.f, 1.f);   // +1
    if (FMath::IsNearlyZero(Axis)) return;

    const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector  Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    AddMovementInput(Dir, -Axis);        // 뒤로
}

void AASPlayerCharacter::MoveRightAxis(const FInputActionValue& Value)
{
    const float Axis =  FMath::Clamp(Value.Get<float>(), -1.f, 1.f);
    if (FMath::IsNearlyZero(Axis)) return;

    const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector  Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
    AddMovementInput(Dir,  Axis);        // 오른쪽
}
void AASPlayerCharacter::MoveLeftAxis(const FInputActionValue& Value)
{
    const float Axis =  FMath::Clamp(Value.Get<float>(), -1.f, 1.f);
    if (FMath::IsNearlyZero(Axis)) return;

    const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector  Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
    AddMovementInput(Dir, -Axis);        // 왼쪽
}

/* 마우스 Y 반전 */
void AASPlayerCharacter::LookUpAxis(const FInputActionValue& Value)
{
    AddControllerPitchInput(-Value.Get<float>());      // 음수 방향으로 입력
}


void AASPlayerCharacter::TurnAxis(const FInputActionValue& Value)
{
    AddControllerYawInput(Value.Get<float>());
}

/* ───────────── Magic helpers ───────────── */
FIntPoint AASPlayerCharacter::GetFacingDir4() const
{
    const FVector Fwd = Controller ? Controller->GetControlRotation().Vector() : GetActorForwardVector();
    return (FMath::Abs(Fwd.X) > FMath::Abs(Fwd.Y))
        ? (Fwd.X >= 0 ? FIntPoint{+1,0} : FIntPoint{-1,0})
        : (Fwd.Y >= 0 ? FIntPoint{0,+1}  : FIntPoint{0,-1});
}

void AASPlayerCharacter::CastFire()
{
    if (!GridMgr) return;

    const FIntPoint SelfCell = GridMgr->WorldToGrid(GetActorLocation());
    const FIntPoint Dir      = GetFacingDir4();        // 바라보는 4방향
    GridMgr->UseFireSpell(SelfCell, Dir);              // 내부에서: SelfFloor–1, Dir Wall–1
}

void AASPlayerCharacter::CastIce()
{
    if (!GridMgr) return;

    const FIntPoint SelfCell = GridMgr->WorldToGrid(GetActorLocation());
    const FIntPoint Dir      = GetFacingDir4();
    const FIntPoint FrontCell = SelfCell + Dir;

    /* 앞 셀이 맵 안이고 Floor 면 거기에 적용, 아니면 자기 셀 */
    const bool bUseFront =
        GridMgr->IsInBounds(FrontCell) &&
        GridMgr->IsFloor(FrontCell);          // 새 helper (아래 참고)

    const FIntPoint Target = bUseFront ? FrontCell : SelfCell;
    GridMgr->UseIceSpell(Target);             // 내부에서: 0→1→2 (최대 2)
}

/* ───────── Action 콜백 빈 틀 ───────── */
void AASPlayerCharacter::OnAttack   (){ /* TODO: Combo / GAS */ }
void AASPlayerCharacter::OnDodge    (){ /* TODO: i-frame roll */ }
void AASPlayerCharacter::OnGuardStart(){ /* bIsGuard=true */ }
void AASPlayerCharacter::OnGuardEnd (){ /* bIsGuard=false */ }
void AASPlayerCharacter::OnLockOn   (){ /* Toggle target lock */ }
void AASPlayerCharacter::OnInteract (){ /* NPC 대화 등 */ }
void AASPlayerCharacter::OnInventory(){ /* 인벤 UI Toggle  */ }
void AASPlayerCharacter::OnPauseESC (){ /* Pause Menu     */ }
