#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AttackTrace.generated.h"

UCLASS()
class ARCANESOULS_API UAnimNotifyState_AttackTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public: // UFUNCTION (public)
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected: // UPROPERTY (protected)
    UPROPERTY(EditAnywhere, Category="Combat|Trace")
    float TraceRadius = 15.f;

    UPROPERTY(EditAnywhere, Category="Combat|Trace")
    FName BaseSocket = "Sword_Base";

    UPROPERTY(EditAnywhere, Category="Combat|Trace")
    FName TipSocket = "Sword_Tip";

private:
    FVector PrevTip = FVector::ZeroVector;

    // 한 프레임 내 중복 히트 방지용 캐시
    TSet<TWeakObjectPtr<AActor>> HitActorsThisFrame;
};
