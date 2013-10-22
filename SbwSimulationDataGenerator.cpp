/*
 * sbwlogic
 *
 * Copyright (C) 2013 Fredrik Ahlberg
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <cstdlib> // for rand

#include "SbwSimulationDataGenerator.h"
#include "SbwAnalyzerSettings.h"

SbwSimulationDataGenerator::SbwSimulationDataGenerator()
{
}

SbwSimulationDataGenerator::~SbwSimulationDataGenerator()
{
}

void SbwSimulationDataGenerator::Initialize(U32 simulation_sample_rate, SbwAnalyzerSettings* settings)
{
#if 0
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mClockGenerator.Init(simulation_sample_rate / 10, simulation_sample_rate);

	mTCK = mJtagSimulationChannels.Add(settings->mTCKChannel, mSimulationSampleRateHz, BIT_LOW);
	mTMS = mJtagSimulationChannels.Add(settings->mTMSChannel, mSimulationSampleRateHz, BIT_LOW);
	mTDI = mJtagSimulationChannels.Add(settings->mTDIChannel, mSimulationSampleRateHz, BIT_LOW);
	mTDO = mJtagSimulationChannels.Add(settings->mTDOChannel, mSimulationSampleRateHz, BIT_LOW);

	if (settings->mTRSTChannel != UNDEFINED_CHANNEL) {
		mTRST = mJtagSimulationChannels.Add(settings->mTRSTChannel, mSimulationSampleRateHz, BIT_LOW);
	} else {
		mTRST = NULL;
	}

	mJtagSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10.0));
#endif
}

U32 SbwSimulationDataGenerator::GenerateSimulationData(U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels)
{
#if 0
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample(largest_sample_requested, sample_rate, mSimulationSampleRateHz);

	while (mTCK->GetCurrentSampleNumber() < adjusted_largest_sample_requested) {
		CreateJtagTransaction();

		mJtagSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod((rand() % 10) * 10.0));
	}

	*simulation_channels = mJtagSimulationChannels.GetArray();
	return mJtagSimulationChannels.GetCount();
#endif
}

void SbwSimulationDataGenerator::CreateJtagTransaction()
{
#if 0
	switch (rand() % 2) {
	case 0:
		MoveState("0100"); // SelectDR
		Scan(rand(), rand(), rand() % 24 + 1);
		MoveState("1111"); // Reset
		break;

	case 1:
		MoveState("0110"); // SelectIR
		Scan(rand(), rand(), rand() % 24 + 1);
		MoveState("1111"); // Reset
		break;
	}
#endif
}

void SbwSimulationDataGenerator::MoveState(const char *tms)
{
#if 0
	mTDI->TransitionIfNeeded(BIT_LOW);
	mTDO->TransitionIfNeeded(BIT_LOW);

	while (*tms) {
		mTMS->TransitionIfNeeded(static_cast<BitState>(*tms++ == '1'));

		mJtagSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
		mTCK->Transition();

		mJtagSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
		mTCK->Transition();
	}
#endif
}

void SbwSimulationDataGenerator::Scan(U32 in, U32 out, U32 bits)
{
#if 0
	mTMS->TransitionIfNeeded(BIT_LOW);

	while (bits--) {
		mTDI->TransitionIfNeeded(static_cast<BitState>(in & 1));
		mTDO->TransitionIfNeeded(static_cast<BitState>(out & 1));

		in >>= 1;
		out >>= 1;

		if (bits == 0) {
			mTMS->Transition();
		}

		mJtagSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
		mTCK->Transition();

		mJtagSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
		mTCK->Transition();
	}
#endif
}

