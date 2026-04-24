// Fill out your copyright notice in the Description page of Project Settings.


#include "Package/PickupActor/PickupActorBase.h"

// Sets default values
APickupActorBase::APickupActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APickupActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
