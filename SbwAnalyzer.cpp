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

#include "SbwAnalyzer.h"
#include "SbwAnalyzerSettings.h"
#include <AnalyzerChannelData.h>
 
SbwAnalyzer::SbwAnalyzer()
:	Analyzer2(),
	mSettings(new SbwAnalyzerSettings()),
	mSimulationInitilized(false),
	mTCK(NULL),
	mTDIO(NULL)
{	
	SetAnalyzerSettings(mSettings.get());
}

SbwAnalyzer::~SbwAnalyzer()
{
	KillThread();
}

void SbwAnalyzer::SetupResults()
{
	mResults.reset(new SbwAnalyzerResults(this, mSettings.get()));
	SetAnalyzerResults(mResults.get());

	mResults->AddChannelBubblesWillAppearOn(mSettings->mTCKChannel);
	mResults->AddChannelBubblesWillAppearOn(mSettings->mTDIOChannel);
}

void SbwAnalyzer::WorkerThread()
{
	Setup();

	mSlot = SbwTMS;
	mState = JtagReset;
	mFirstSample = mTCK->GetSampleNumber();
	mDataIn = mDataOut = 0;
	mBits = 0;

	if (mTCK->GetBitState() == 0) {
		mTCK->AdvanceToNextEdge();
	}

	for ( ; ; ) {
		mTCK->AdvanceToNextEdge();
		mCurrentSample = mTCK->GetSampleNumber();

		ProcessStep();

		mTCK->AdvanceToNextEdge();
		if (mTCK->GetSampleNumber() - mCurrentSample > mTCKTimeout) {
			mResults->AddMarker(mTCK->GetSampleNumber(), AnalyzerResults::Stop, mSettings->mTCKChannel);
			mSlot = SbwIdle;
			mState = JtagReset;
		}

		CheckIfThreadShouldExit();
	}
}

void SbwAnalyzer::Setup()
{
	mTCK = GetAnalyzerChannelData(mSettings->mTCKChannel);
	mTDIO = GetAnalyzerChannelData(mSettings->mTDIOChannel);

	mTCKTimeout = GetSampleRate() / 14286; // 7 us
	mTDOSkip = GetSampleRate() / 1e7; // 100 ns
}

void SbwAnalyzer::ProcessJtag()
{
	// Update JTAG state machine
	enum JtagState next_state = mState;

	switch (mState) {
	case JtagReset:
		next_state = mTMSValue ? JtagReset : JtagIdle;
		break;

	case JtagIdle:
		next_state = mTMSValue ? JtagSelectDR : JtagIdle;
		break;

	case JtagSelectDR:
		next_state = mTMSValue ? JtagSelectIR : JtagCaptureDR;
		break;

	case JtagCaptureDR:
		next_state = mTMSValue ? JtagExit1DR : JtagShiftDR;
		break;

	case JtagShiftDR:
		next_state = mTMSValue ? JtagExit1DR : JtagShiftDR;
		break;

	case JtagExit1DR:
		next_state = mTMSValue ? JtagUpdateDR : JtagPauseDR;
		break;

	case JtagPauseDR:
		next_state = mTMSValue ? JtagExit2DR : JtagPauseDR;
		break;

	case JtagExit2DR:
		next_state = mTMSValue ? JtagUpdateDR : JtagShiftDR;
		break;

	case JtagUpdateDR:
		next_state = mTMSValue ? JtagSelectDR : JtagIdle;
		break;

	case JtagSelectIR:
		next_state = mTMSValue ? JtagReset : JtagCaptureIR;
		break;

	case JtagCaptureIR:
		next_state = mTMSValue ? JtagExit1IR : JtagShiftIR;
		break;

	case JtagShiftIR:
		next_state = mTMSValue ? JtagExit1IR : JtagShiftIR;
		break;

	case JtagExit1IR:
		next_state = mTMSValue ? JtagUpdateIR : JtagPauseIR;
		break;

	case JtagPauseIR:
		next_state = mTMSValue ? JtagExit2IR : JtagPauseIR;
		break;

	case JtagExit2IR:
		next_state = mTMSValue ? JtagUpdateIR : JtagShiftIR;
		break;

	case JtagUpdateIR:
		next_state = mTMSValue ? JtagSelectDR : JtagIdle;
		break;
	}

	if (next_state != mState) {
		// transition. yay. flush the current transaction.

		Frame result_frame;
		result_frame.mStartingSampleInclusive = mFirstSample;
		result_frame.mEndingSampleInclusive = mTCK->GetSampleNumber();
		result_frame.mData1 = 0 ? FlipWord(mDataIn, mBits) : mDataIn;
		result_frame.mData2 = 0 ? FlipWord(mDataOut, mBits) : mDataOut;
		result_frame.mFlags = mState;

		U64 frame_id = mResults->AddFrame(result_frame);
		mResults->CommitResults();

		mFirstSample = mTCK->GetSampleNumber();
		mState = next_state;
		mDataIn = mDataOut = 0;
		mBits = 0;
	}
}

void SbwAnalyzer::ProcessStep()
{
	mTDIO->AdvanceToAbsPosition(mCurrentSample);

	mResults->AddMarker(mCurrentSample, AnalyzerResults::DownArrow, mSettings->mTCKChannel);

	switch (mSlot) {
	case SbwIdle:
		mSlot = SbwTMS;
		break;

	case SbwTMS:
		mSlot = SbwTDI;
		mTMSValue = mTDIO->GetBitState();
		break;

	case SbwTDI:
		mSlot = SbwTDO;
		if (mState == JtagShiftDR || mState == JtagShiftIR) {
			mDataIn = (mDataIn << 1) | (mTDIO->GetBitState() ? 1 : 0);
			mResults->AddMarker(mCurrentSample, AnalyzerResults::Dot, mSettings->mTDIOChannel);
		}
		break;

	case SbwTDO:
		mSlot = SbwTMS;
		if (mState == JtagShiftDR || mState == JtagShiftIR) {
			// Skip a jiffy before sampling TDIO
			mTDIO->Advance(mTDOSkip);

			// Do stuff.
			mDataOut = (mDataOut << 1) | (mTDIO->GetBitState() ? 1 : 0);
			mResults->AddMarker(mTDIO->GetSampleNumber(), AnalyzerResults::Dot, mSettings->mTDIOChannel);
			mBits++;
		}
		ProcessJtag();
	}
}

// Register contents are shifted LSB first, so this
// helper flips the words for us.
U64 SbwAnalyzer::FlipWord(U64 word, U32 bits)
{
	U64 result = 0;

	for (int idx=0; idx<bits; idx++) {
		if (word & (1 << idx)) {
			result |= (1 << (bits-idx-1));
		}
	}

	return result;
}

bool SbwAnalyzer::NeedsRerun()
{
	return false;
}

U32 SbwAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if (mSimulationInitilized == false) {
		mSimulationDataGenerator.Initialize(GetSimulationSampleRate(), mSettings.get());
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData(minimum_sample_index, device_sample_rate, simulation_channels);
}


U32 SbwAnalyzer::GetMinimumSampleRateHz()
{
	return 10000; //we don't have any idea, depends on the JTAG TCK rate, etc.; return the lowest rate.
}

const char* SbwAnalyzer::GetAnalyzerName() const
{
	return "Spy-Bi-Wire";
}

const char* GetAnalyzerName()
{
	return "Spy-Bi-Wire";
}

Analyzer* CreateAnalyzer()
{
	return new SbwAnalyzer();
}

void DestroyAnalyzer(Analyzer* analyzer)
{
	delete analyzer;
}
