#include "ASPlayerCharacter.h"

#include "ArcaneSouls/Systems/GridPuzzle/GridPuzzleManagerComponent.h"
#include "Camera/CameraComponent.h"
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
}

/* ───────────── Input Bindings ───────────── */
void AASPlayerCharacter::SetupPlayerInputComponent(UInputComponent* IC)
{
    Super::SetupPlayerInputComponent(IC);

    /* Movement */
    IC->BindAxis("MoveForward", this, &AASPlayerCharacter::MoveForward);
    IC->BindAxis("MoveRight",   this, &AASPlayerCharacter::MoveRight);
    IC->BindAxis("Turn",        this, &AASPlayerCharacter::AddControllerYawInput);
    IC->BindAxis("LookUp",      this, &AASPlayerCharacter::AddControllerPitchInput);

    /* Actions */
    IC->BindAction("Jump",     IE_Pressed,  this, &ACharacter::Jump);
    IC->BindAction("Jump",     IE_Released, this, &ACharacter::StopJumping);
    IC->BindAction("CastFire", IE_Pressed,  this, &AASPlayerCharacter::CastFire);
    IC->BindAction("CastIce",  IE_Pressed,  this, &AASPlayerCharacter::CastIce);
}

/* ───────────── Movement helpers ─────────── */
void AASPlayerCharacter::MoveForward(float Value)
{
    if (Controller && Value!=0.f)
    {
        const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
        const FVector  Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        AddMovementInput(Dir, Value);
    }
}

void AASPlayerCharacter::MoveRight(float Value)
{
    if (Controller && Value!=0.f)
    {
        const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
        const FVector  Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(Dir, Value);
    }
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
    const FIntPoint Cell = GridMgr->WorldToGrid(GetActorLocation());
    const FIntPoint Dir  = GetFacingDir4();
    GridMgr->UseFireSpell(Cell, Dir);
}

void AASPlayerCharacter::CastIce()
{
    if (!GridMgr) return;
    const FIntPoint Cell = GridMgr->WorldToGrid(GetActorLocation());
    GridMgr->UseIceSpell(Cell);
}
