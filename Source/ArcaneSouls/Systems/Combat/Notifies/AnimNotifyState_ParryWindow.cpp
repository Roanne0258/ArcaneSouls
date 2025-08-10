// Source/ArcaneSouls/Systems/Combat/Notifies/AnimNotifyState_ParryWindow.cpp
#include "AnimNotifyState_ParryWindow.h"
#include "Kismet/GameplayStatics.h"
#include "ArcaneSouls/Systems/Combat/Data/ASParryConstants.h"
#include "ArcaneSouls/Characters/Player/ASPlayerCharacter.h"
#include "ArcaneSouls/Core/ASLogChannels.h"
#include "ArcaneSouls/Systems/Combat/Components/ASParryComponent.h"

void UAnimNotifyState_ParryWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    if (!MeshComp) return;

    UWorld* World = MeshComp->GetWorld();
    if (!World) return;

    const float W = ASParry::PerfectParryWindow;     // ≈0.033s
    const float T0 = World->GetTimeSeconds();        // 노티 시작 절대시간
    const float Norm = FMath::Clamp(ActivationPointNormalized, 0.f, 1.f);

    // 노티 길이 내에서 원하는 "중심 시간" 잡기
    float Center = T0 + (TotalDuration * Norm) + ActivationBiasSeconds;

    // 윈도우가 노티 범위를 벗어나지 않도록 클램프
    const float MinStart = T0;
    const float MaxStart = T0 + FMath::Max(0.f, TotalDuration - W);
    float Start = FMath::Clamp(Center - 0.5f * W, MinStart, MaxStart);

    FParryWindow Window;
    Window.StartTime = Start;
    Window.EndTime   = Start + W;
    Window.Element   = Element;

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
    {
        if (AASPlayerCharacter* PlayerChar = Cast<AASPlayerCharacter>(PC->GetPawn()))
        {
            if (UASParryComponent* PlayerParryComp = PlayerChar->FindComponentByClass<UASParryComponent>())
            {
                PlayerParryComp->SetParryWindow(Window);
            }
        }
    }

    UE_LOG(LogAS_Combat, Verbose, TEXT("[ParryNotify] Start=%.4f End=%.4f Center=%.4f Norm=%.2f Bias=%.3f Dur=%.3f"),
        Window.StartTime, Window.EndTime, Center, Norm, ActivationBiasSeconds, TotalDuration);
}

void UAnimNotifyState_ParryWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

}
