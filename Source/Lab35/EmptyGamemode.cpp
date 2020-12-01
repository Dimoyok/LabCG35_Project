// Fill out your copyright notice in the Description page of Project Settings.


#include "EmptyGamemode.h"

AEmptyGamemode::AEmptyGamemode()
{
	DefaultPawnClass = AFlyCam::StaticClass();
	GameSessionClass = nullptr;
	HUDClass = nullptr;
}