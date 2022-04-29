/*
  ==============================================================================

    AdsrData.cpp
    Created: 25 Feb 2022 9:03:58pm
    Author:  lukeb_000

  ==============================================================================
*/

#include "AdsrData.h"

void AdsrData::updateADSR(const float attack, const float decay, const float sustain, const float release)
{
    adsrParams.attack = attack;
    adsrParams.decay = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;

    setParameters(adsrParams);
}
