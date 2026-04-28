// Fill out your copyright notice in the Description page of Project Settings.


#include "Package/PickupActor/PickupActorBase.h"

#include "Character/BaseCanPickCharacter.h"
#include "Components/SphereComponent.h"
#include "DataAsset/ItemDataAsset.h"
#include "Package/UserPackageComponent.h"

// Sets default values
APickupActorBase::APickupActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractorComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InteractorComponent"));
	InteractorComponent->SetupAttachment(Root);
	InteractorComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractorComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractorComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractorComponent->SetSphereRadius(100.f);
	
	InteractorComponent->OnComponentBeginOverlap.AddDynamic(this,&APickupActorBase::OnBeginOverlap);
	InteractorComponent->OnComponentEndOverlap.AddDynamic(this,&APickupActorBase::OnEndOverlap);
}

bool APickupActorBase::CanInteract(AActor* Interactor) const
{
	EPickupFailReason Reason = EPickupFailReason::None;
	return ValidatePickup(Interactor,Reason);
}

bool APickupActorBase::TryPickup(AActor* Interactor)
{
	EPickupFailReason Reason = EPickupFailReason::None;
	//无法通过拾取校验时（拾取校验负责修改失败原因）
	if (!ValidatePickup(Interactor,Reason))
	{
		PickupFailed.Broadcast(Interactor,Reason);
		OnPickupFailed(Interactor,Reason);
		return false;
	}
	
	//26427 Package组件完成后 PickupActor尝试调用Package组件
	UUserPackageComponent* Package = Interactor->FindComponentByClass<UUserPackageComponent>();
	if (!Package)
	{
		//Interactor无Package导致失败
		Reason = EPickupFailReason::AddToInventoryFailed;
		PickupFailed.Broadcast(Interactor,Reason);
		OnPickupFailed(Interactor,Reason);
		return false;
	}
	
	//清除原有蓝图拾取逻辑
	/*
	//通过拾取校验
	int32 AcceptedCount = 0;
	const bool bAdded = K2_RequestAddItemToInteractor(Interactor,ItemData->ItemID,Count,AcceptedCount);
	
	//拾取者无法拾起（拾取者自身原因）
	if (!bAdded || AcceptedCount <= 0)
	{
		Reason = EPickupFailReason::AddToInventoryFailed;
		PickupFailed.Broadcast(Interactor,Reason);
		OnPickupFailed(Interactor,Reason);
		return false;
	} 
	 */

	//调用添加逻辑获取添加数量
	const int32 AcceptedCount = Package->AddItem(ItemData->ItemID,Count);
	//未添加成功
	if (AcceptedCount <= 0)
	{
		Reason = EPickupFailReason::AddToInventoryFailed;
		PickupFailed.Broadcast(Interactor,Reason);
		OnPickupFailed(Interactor,Reason);
		return false;
	}
	//拾取成功
	PickupSucceeded.Broadcast(Interactor,AcceptedCount);
	OnPickupSuccess(Interactor,AcceptedCount);
	if (bDestroyOnPickupSuccess)
	{
		Destroy();
	}
	return true;
}

FText APickupActorBase::GetInteractorText() const
{
	//如果拾取物的数据资产有误 则不能拾取
	if (!ItemData)
	{
		return FText::FromString(TEXT("Can't pickup"));
	}
	
	//成功拾取后返回文本 格式为 [拾取 “DisplayName” x “count”]
	return FText::Format(FText::FromString(TEXT("拾取 {0} x {1}")),ItemData->DisplayName,FText::AsNumber(Count));
}

// Called when the game starts or when spawned
void APickupActorBase::BeginPlay()
{
	Super::BeginPlay();
}

void APickupActorBase::OnPickupSuccess_Implementation(AActor* Interactor, int32 AcceptedCount)
{
}

void APickupActorBase::OnPickupFailed_Implementation(AActor* Interactor, EPickupFailReason Reason)
{
}

bool APickupActorBase::ValidatePickup(AActor* Interactor, EPickupFailReason& FailReason) const
{
	if (!Interactor)
	{
		FailReason = EPickupFailReason::OutOfRange;
		return false;
	}
	if (!ItemData || ItemData->ItemID.IsNone())
	{
		FailReason = EPickupFailReason::InvalidItemData;
		return false;
	}
	if (Count<=0)
	{
		FailReason = EPickupFailReason::InvalidCount;
		return false;
	}
	if (!IsInteractorInRange(Interactor))
	{
		FailReason = EPickupFailReason::OutOfRange;
		return false;
	}
	FailReason = EPickupFailReason::None;
	return true;
}

bool APickupActorBase::IsInteractorInRange(AActor* Interactor) const
{
	if (!Interactor)
	{
		return false;
	}
	//计算距离的平方 省略正负 性能优化
	const float Distance = FVector::DistSquared(Interactor->GetActorLocation(),GetActorLocation());
	//计算距离平方和预设可拾取距离平方的大小
	return Distance <= FMath::Square(MaxInteractorDis);
}

void APickupActorBase::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABaseCanPickCharacter* Character = Cast<ABaseCanPickCharacter>(OtherActor))
	{
		Character->SetCurrentPick(this);
	}
}

void APickupActorBase::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABaseCanPickCharacter* Character = Cast<ABaseCanPickCharacter>(OtherActor))
	{
		Character->ClearCurrentPick(this);
	}
}
