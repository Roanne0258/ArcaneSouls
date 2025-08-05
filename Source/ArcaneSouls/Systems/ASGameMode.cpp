#include "ASGameMode.h"
#include "ArcaneSouls/Characters/Player/ASPlayerCharacter.h"          // Default Pawn
#include "ArcaneSouls/Characters/Player/ASPlayerController.h"         // 커스텀 Controller (Enhanced Input 용)
#include "UObject/ConstructorHelpers.h"

AASGameMode::AASGameMode()
{
	/* Default Pawn */
	static ConstructorHelpers::FClassFinder<APawn> PawnBP(TEXT("/Game/Characters/Player/Blueprints/BP_ASPlayerCharacter"));
	if (PawnBP.Succeeded())
		DefaultPawnClass = PawnBP.Class;

	/* Player Controller */
	PlayerControllerClass = AASPlayerController::StaticClass();

	/* HUD / GameState 등 필요 시 추가
	   HUDClass       = AASHUD::StaticClass();
	   GameStateClass = AASGameState::StaticClass();
	*/
}