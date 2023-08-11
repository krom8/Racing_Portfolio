// Fill out your copyright notice in the Description page of Project Settings.


#include "General/RearWheel.h"

URearWheel::URearWheel()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
	bAffectedBySteering = false;
}


URearWheel::~URearWheel()
{

}