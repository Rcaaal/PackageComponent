// Fill out your copyright notice in the Description page of Project Settings.


#include "PkgGameStateBase.h"

#include "Component/CountComponent.h"

APkgGameStateBase::APkgGameStateBase()
{
	CountComponent = CreateDefaultSubobject<UCountComponent>(TEXT("CountComponent"));
}
