#include "PlacementHelperActor.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogASPlacement);

namespace PlacementConst
{
	constexpr float TraceDepth = 10000.f;   // 라인트레이스 최대 깊이
}

void APlacementHelperActor::AlignToGround()
{
#if WITH_EDITOR
	if (!TargetComponent)
	{
		TargetComponent = GetRootComponent();
	}
	if (!TargetComponent) return;

	const FVector start = TargetComponent->GetComponentLocation();
	const FVector end   = start - FVector(0.f, 0.f, PlacementConst::TraceDepth);

	FHitResult hit;
	FCollisionQueryParams params(SCENE_QUERY_STAT(AlignToGround), true);
	params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_WorldStatic, params))
	{
		TargetComponent->SetWorldLocation(hit.Location);
		UE_LOG(LogASPlacement, Log, TEXT("Aligned to %s"), *hit.Location.ToString());
#if !(UE_BUILD_SHIPPING)
		DrawDebugLine(GetWorld(), start, hit.Location, FColor::Green, false, 2.f);
#endif
	}
	else
	{
		UE_LOG(LogASPlacement, Warning, TEXT("Ground not found."));
	}
#endif
}
