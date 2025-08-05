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

    EIC->BindAction(IA_CastFire, ETriggerEvent::Started, this, &AASPlayerCharacter::CastFire);
    EIC->BindAction(IA_CastIce , ETriggerEvent::Started, this, &AASPlayerCharacter::CastIce);
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
    CastMagic();
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

float AASPlayerCharacter::GetMagicPower() const
{
    return MagicPower;
}


void AASPlayerCharacter::CastMagic()
{
    if (!MagicProjectileClass) return;

    // 1) Spawn 위치/회전
    FVector SpawnLoc = GetMesh()->GetSocketLocation(HandSocketName);
    FRotator SpawnRot = GetControlRotation();

    // 2) Spawn 파라미터
    FActorSpawnParameters Params;
    Params.Owner     = this;
    Params.Instigator= this;

    // 3) 프로젝타일 스폰
    AASProjectileBase* Proj = GetWorld()
        ->SpawnActor<AASProjectileBase>(MagicProjectileClass,
                                        SpawnLoc, SpawnRot, Params);
    if (!Proj) return;

    // 4) InitProjectile 호출
    Proj->InitProjectile(
        MagicProjectileSpeed,       // InSpeed
        MagicBaseDamage,            // InBaseDamage
        MagicDamageScale,           // InDamageScale
        EParryElementType::None     // InElement (속성 마법이 아니면 None)
    );

    UE_LOG(LogAS_Magic, Log, TEXT("CastMagic: Spawned %s (Speed=%.0f, BaseDmg=%.1f)"),
           *Proj->GetName(), MagicProjectileSpeed, MagicBaseDamage);
}
