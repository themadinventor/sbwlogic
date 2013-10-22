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

#ifndef SBW_ANALYZER_H
#define SBW_ANALYZER_H

#include <Analyzer.h>
#include "SbwAnalyzerResults.h"
#include "SbwSimulationDataGenerator.h"

enum SbwState {
	SbwTMS,
	SbwTDI,
	SbwTDO,
	SbwIdle
};

enum JtagState {
	JtagReset,
	JtagIdle,

	JtagSelectDR,
	JtagCaptureDR,
	JtagShiftDR,
	JtagExit1DR,
	JtagPauseDR,
	JtagExit2DR,
	JtagUpdateDR,

	JtagSelectIR,
	JtagCaptureIR,
	JtagShiftIR,
	JtagExit1IR,
	JtagPauseIR,
	JtagExit2IR,
	JtagUpdateIR
};

class SbwAnalyzerSettings;
class SbwAnalyzer : public Analyzer2
{
public:
	SbwAnalyzer();
	virtual ~SbwAnalyzer();
	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //functions
	void Setup();
	void ProcessJtag();
	void ProcessStep();
	U64 FlipWord(U64 word, U32 bits);
	
protected:  //vars
	std::auto_ptr< SbwAnalyzerSettings > mSettings;
	std::auto_ptr< SbwAnalyzerResults > mResults;
	bool mSimulationInitilized;
	SbwSimulationDataGenerator mSimulationDataGenerator;

	AnalyzerChannelData* mTCK; 
	AnalyzerChannelData* mTDIO;

	U64 mCurrentSample, mFirstSample, mTCKTimeout, mTDOSkip;

	enum SbwState mSlot;
	enum JtagState mState;
	bool mTMSValue;
	U64 mDataIn, mDataOut;
	U32 mBits;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //SBW_ANALYZER_H
