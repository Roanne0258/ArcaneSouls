#include "GridCellBaseActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"

///////////////////////////////////////////////////////////
// 생성자 및 초기화
///////////////////////////////////////////////////////////

AGridCellBaseActor::AGridCellBaseActor()
{
    Mesh = Cast<UStaticMeshComponent>(GetRootComponent());

    HealthText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthText"));
    HealthText->SetupAttachment(Mesh);
    HealthText->SetHorizontalAlignment(EHTA_Center);
    HealthText->SetVerticalAlignment(EVRTA_TextCenter);
    HealthText->SetWorldSize(30.f);
    HealthText->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
    HealthText->SetMobility(EComponentMobility::Movable);
}

void AGridCellBaseActor::BeginPlay()
{
    Super::BeginPlay();
    RefreshVisual();
}

///////////////////////////////////////////////////////////
// 시각 갱신
///////////////////////////////////////////////////////////

void AGridCellBaseActor::RefreshVisual()
{
    Health = FMath::Clamp(Health, 0, 3);
    HealthText->SetText(FText::AsNumber(Health));

    const float Ratio = Health / 3.f;
    const FLinearColor Col = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Blue, Ratio);
    HealthText->SetTextRenderColor(Col.ToFColor(false));
}

///////////////////////////////////////////////////////////
// 데미지 처리
///////////////////////////////////////////////////////////

void AGridCellBaseActor::ApplyGridDamage_Implementation(int32 Amount)
{
    Health = FMath::Max(0, Health - Amount);
    RefreshVisual();

    if (Health <= 0)
    {
        Destroy();
    }
}

///////////////////////////////////////////////////////////
// 에디터 전용 처리
///////////////////////////////////////////////////////////

#if WITH_EDITOR
void AGridCellBaseActor::PostEditChangeProperty(FPropertyChangedEvent& e)
{
    Super::PostEditChangeProperty(e);

    if (e.Property && e.Property->GetFName() == GET_MEMBER_NAME_CHECKED(AGridCellBaseActor, Health))
    {
        RefreshVisual();
    }
}
#endif
