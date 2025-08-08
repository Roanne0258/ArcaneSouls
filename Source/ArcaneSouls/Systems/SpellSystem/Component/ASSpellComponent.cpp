#include "ASSpellComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Engine/DataTable.h"
#include "ArcaneSouls/Characters/Player/ASPlayerCharacter.h"
#include "ArcaneSouls/Systems/SpellSystem/Projectiles/ASProjectileBase.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY(LogAS_Spell);

UASSpellComponent::UASSpellComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UASSpellComponent::BeginPlay()
{
	Super::BeginPlay();
	// 처음 슬롯(0) 데이터 로드
	if (SpellRowNames.IsValidIndex(0))
	{
		SetActiveSpellIndex(0);
	}
}

void UASSpellComponent::SetActiveSpellIndex(int32 NewIndex)
{
	if (!SpellRowNames.IsValidIndex(NewIndex)) return;
	ActiveSpellIndex = NewIndex;
	LoadSpellData(SpellRowNames[ActiveSpellIndex]);
}

void UASSpellComponent::LoadSpellData(const FName& RowName)
{
	if (!SpellDataTable) return;

	static const FString Context = TEXT("SpellDataLoad");
	if (FSpellData const* Data = SpellDataTable->FindRow<FSpellData>(RowName, Context))
	{
		// Row에서 바로 읽어오기
		ProjectileClass = Data->ProjectileClass;
		HandSocketName  = Data->HandSocketName;
		ProjectileSpeed = Data->ProjectileSpeed;
		BaseDamage      = Data->BaseDamage;
		DamageScale     = Data->DamageScale;
	}
}

void UASSpellComponent::StartCharge()
{
	UE_LOG(LogTemp, Warning, TEXT("StartCharge"));
	if (bIsCharging) return;
	bIsCharging = true;
	CurrentStacks = 0;
	GetWorld()->GetTimerManager()
		.SetTimer(ChargeTimerHandle, this, &UASSpellComponent::HandleChargeStack, ChargeInterval, true);
}

void UASSpellComponent::HandleChargeStack()
{
	UE_LOG(LogAS_Spell, Warning, TEXT("[Spell] HandleChargeStack() called. ChargeOrbClass=%s"), *GetNameSafe(ChargeOrbClass));
	auto* PC = Cast<AASPlayerCharacter>(GetOwner());
	if (!PC || CurrentStacks >= MaxChargeStacks || PC->GetCurrentMP() < MPPerStack)
	{
		GetWorld()->GetTimerManager().ClearTimer(ChargeTimerHandle);
		return;
	}

	PC->ModifyMP(-MPPerStack);
	// Orb 스폰 (원형 궤도 연출은 여기서 위치 계산)
	FVector OwnerLoc = PC->GetActorLocation();
	float Angle = 360.f * CurrentStacks / MaxChargeStacks;
	FVector Offset = FVector(FMath::Cos(FMath::DegreesToRadians(Angle)), FMath::Sin(FMath::DegreesToRadians(Angle)), 0) * 100.f;
	FActorSpawnParameters Params; Params.Owner = PC;
	AActor* Orb = GetWorld()->SpawnActor<AActor>(ChargeOrbClass, OwnerLoc + Offset + FVector(0,0,80), FRotator::ZeroRotator, Params);
	ChargeOrbs.Add(Orb);
	CurrentStacks++;
}

void UASSpellComponent::CancelCharge()
{
	if (!bIsCharging) return;
	bIsCharging = false;
	GetWorld()->GetTimerManager().ClearTimer(ChargeTimerHandle);

	auto* PC = Cast<AASPlayerCharacter>(GetOwner());
	if (PC) PC->ModifyMP(MPPerStack * CurrentStacks);

	for (AActor* Orb : ChargeOrbs) if (Orb) Orb->Destroy();
	ChargeOrbs.Empty();
	CurrentStacks = 0;
}

void UASSpellComponent::ReleaseCharge()
{
	// 1) 호출 로그 (선택)
	UE_LOG(LogAS_Spell, Warning, TEXT("[Spell] ReleaseCharge() called, CurrentStacks=%d"), CurrentStacks);

	if (!bIsCharging) 
		return;

	bIsCharging = false;
	GetWorld()->GetTimerManager().ClearTimer(ChargeTimerHandle);

	// 2) 스택이 없으면 즉시 발사
	if (CurrentStacks == 0)
	{
		UE_LOG(LogAS_Spell, Warning, TEXT("[Spell] No stacks → casting instant"));
		CastInstant();
		return;
	}

	// 3) 스택 발사 로직 (기존)
	auto* PC = Cast<AASPlayerCharacter>(GetOwner());
	for (AActor* Orb : ChargeOrbs)
	{
		if (!Orb || !ProjectileClass || !PC) 
			continue;

		FVector Loc = Orb->GetActorLocation();
		FRotator Rot = PC->GetController() 
					   ? PC->GetController()->GetControlRotation() 
					   : PC->GetActorRotation();

		FActorSpawnParameters Params;
		Params.Owner = PC;

		if (auto* Bolt = GetWorld()->SpawnActor<AASProjectileBase>(ProjectileClass, Loc, Rot, Params))
		{
			Bolt->InitVelocity(Rot.Vector() * ProjectileSpeed);
			Bolt->SetDamage(BaseDamage * DamageScale * PC->GetMagicPower());
		}
		Orb->Destroy();
	}

	ChargeOrbs.Empty();
	CurrentStacks = 0;
}



void UASSpellComponent::CastInstant()
{
	UE_LOG(LogTemp, Warning, TEXT("CastInstant"));
	// 1) 호출 여부 확인
	UE_LOG(LogAS_Spell, Warning, TEXT("CastInstant() called on %s"), *GetOwner()->GetName());

	// 2) 프로퍼티 값 확인
	UE_LOG(LogAS_Spell, Warning,
		TEXT("  ProjectileClass = %s"),
		*GetNameSafe(ProjectileClass));

	if (!ProjectileClass)
	{
		UE_LOG(LogAS_Spell, Error, TEXT("  → ERROR: ProjectileClass is nullptr!"));
		return;
	}

	// 3) 캐릭터 & 회전 체크
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner)
	{
		UE_LOG(LogAS_Spell, Error, TEXT("  → ERROR: Owner is not ACharacter"));
		return;
	}
	AController* Ctrl = CharacterOwner->GetController();
	if (!Ctrl)
	{
		UE_LOG(LogAS_Spell, Warning, TEXT("  No Controller, using Actor rotation"));
	}

	// 4) 스폰 위치 체크
	USkeletalMeshComponent* MeshComp = CharacterOwner->FindComponentByClass<USkeletalMeshComponent>();
	if (!MeshComp)
	{
		UE_LOG(LogAS_Spell, Error, TEXT("  → ERROR: No SkeletalMeshComponent found on owner"));
		return;
	}
	const FVector SpawnLoc = MeshComp->GetSocketLocation(HandSocketName);
	UE_LOG(LogAS_Spell, Warning,
		TEXT("  SpawnLoc = %s, HandSocketName = %s"),
		*SpawnLoc.ToString(),
		*HandSocketName.ToString());

	// 5) 실제 스폰 시도
	FActorSpawnParameters Params;
	Params.Owner = CharacterOwner;
	AASProjectileBase* Bolt = GetWorld()->SpawnActor<AASProjectileBase>(
		ProjectileClass, SpawnLoc, Ctrl ? Ctrl->GetControlRotation() : CharacterOwner->GetActorRotation(), Params);

	if (!Bolt)
	{
		UE_LOG(LogAS_Spell, Error, TEXT("  → SpawnActor failed!"));
	}
	else
	{
		Bolt->InitVelocity((Ctrl ? Ctrl->GetControlRotation() : CharacterOwner->GetActorRotation()).Vector() * ProjectileSpeed);
		Bolt->SetDamage(BaseDamage * DamageScale * GetMagicPower());
		UE_LOG(LogAS_Spell, Warning, TEXT("  → Spawned Bolt %s"), *Bolt->GetName());
	}
}

