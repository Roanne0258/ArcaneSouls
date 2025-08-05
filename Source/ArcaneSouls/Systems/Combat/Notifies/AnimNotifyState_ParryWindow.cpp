// ======================================================================
//  AnimNotifyState_ParryWindow.cpp   –   Arcane Souls 패링 판정 구간 노티파이
// ======================================================================

#include "AnimNotifyState_ParryWindow.h"
#include "ArcaneSouls/Characters/ASCharacterBase.h"
#include "ArcaneSouls/Characters/Player/ASPlayerCharacter.h"
#include "ArcaneSouls/Systems/Combat/Components/ASParryComponent.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotifyState_ParryWindow::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration)
{
	const float CurrentWorldTime = UGameplayStatics::GetTimeSeconds(MeshComp);
	FParryWindow Window;
	Window.StartTime = CurrentWorldTime;
	Window.EndTime   = CurrentWorldTime + TotalDuration;
	Window.Element   = Element;

	// 플레이어(방어자) ParryComponent에 윈도우 세팅
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(MeshComp->GetWorld(), 0))
	{
		if (AASPlayerCharacter* PlayerChar = Cast<AASPlayerCharacter>(PC->GetPawn()))
		{
			if (UASParryComponent* PlayerParryComp = PlayerChar->FindComponentByClass<UASParryComponent>())
			{
				PlayerParryComp->SetParryWindow(Window);
			}
		}
	}
}

void UAnimNotifyState_ParryWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;
	AASCharacterBase* OwnerChar = Cast<AASCharacterBase>(MeshComp->GetOwner());
	if (!OwnerChar) return;
	UASParryComponent* ParryComp = OwnerChar->FindComponentByClass<UASParryComponent>();
	if (!ParryComp) return;

	ParryComp->ClearParryWindow();
}
