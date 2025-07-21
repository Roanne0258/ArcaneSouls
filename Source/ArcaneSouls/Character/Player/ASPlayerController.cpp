// =============================================================
//  AASPlayerController.cpp
// =============================================================
#include "ASPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AASPlayerController::AASPlayerController()
{
	/* 기본 마우스 커서 설정 */
	bShowMouseCursor   = false;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/** IMC(플레이어) 등록 */
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSys =
			LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_Player)
			{
				SubSys->AddMappingContext(IMC_Player, /*Priority=*/0);
			}
		}
	}
}

void AASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		/* ESC 키 바인딩 → HandlePause */
		if (IA_ESC)
		{
			EIC->BindAction(IA_ESC,
							ETriggerEvent::Started,
							this,
							&AASPlayerController::HandlePause);
		}
	}
}

/* ───────────── ESC 콜백 ───────────── */
void AASPlayerController::HandlePause(const FInputActionValue&)
{
	/* BP에서 메뉴 열/닫기 구현 */
	TogglePauseMenu();

	/* 필요하면 기본 Pause 토글도 함께 */
	// SetPause(!IsPaused());
}
