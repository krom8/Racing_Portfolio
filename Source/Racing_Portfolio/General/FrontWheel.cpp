// Fill out your copyright notice in the Description page of Project Settings.


#include "General/FrontWheel.h"

UFrontWheel::UFrontWheel()
{
	AxleType = EAxleType::Front;
	bAffectedByHandbrake = false;
	bAffectedByEngine = true;
	bAffectedBySteering = true;
}

UFrontWheel::~UFrontWheel()
{

}