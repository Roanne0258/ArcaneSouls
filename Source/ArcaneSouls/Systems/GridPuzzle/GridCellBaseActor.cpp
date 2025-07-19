#include "GridCellBaseActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"

AGridCellBaseActor::AGridCellBaseActor()
{
	/** 1) 부모가 만든 MeshComp를 재참조 */
	Mesh = Cast<UStaticMeshComponent>(GetRootComponent());   // nullptr 방지용 캐스팅

	/** 2) 체력 숫자 텍스트만 추가 */
	HealthText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthText"));
	HealthText->SetupAttachment(Mesh);                       // Root에 부착
	HealthText->SetHorizontalAlignment(EHTA_Center);
	HealthText->SetVerticalAlignment(EVRTA_TextCenter);
	HealthText->SetWorldSize(30.f);
	HealthText->SetRelativeLocation({0.f, 0.f, 60.f});
	HealthText->SetMobility(EComponentMobility::Movable);    // 부모와 일치
}

void AGridCellBaseActor::RefreshVisual()
{
	Health = FMath::Clamp(Health, 0, 3);
	HealthText->SetText(FText::AsNumber(Health));

	/* 색상 예시: 빨강(1)~파랑(3) 그라데이션 */
	const float Ratio = Health / 3.f;
	const FLinearColor Col = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Blue, Ratio);
	HealthText->SetTextRenderColor(Col.ToFColor(false));
}

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
