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

#ifndef SBW_SIMULATION_DATA_GENERATOR
#define SBW_SIMULATION_DATA_GENERATOR

#include <AnalyzerHelpers.h>

class SbwAnalyzerSettings;

class SbwSimulationDataGenerator
{
public:
	SbwSimulationDataGenerator();
	~SbwSimulationDataGenerator();

	void Initialize(U32 simulation_sample_rate, SbwAnalyzerSettings* settings);
	U32 GenerateSimulationData(U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels);

protected:
	SbwAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected: //Jtag specific
	ClockGenerator mClockGenerator;

	void CreateJtagTransaction();
	void MoveState(const char *tms);
	void Scan(U32 in, U32 out, U32 bits);

	SimulationChannelDescriptorGroup mJtagSimulationChannels;
	SimulationChannelDescriptor* mTCK;
	SimulationChannelDescriptor* mTMS;
	SimulationChannelDescriptor* mTDI;
	SimulationChannelDescriptor* mTDO;
	SimulationChannelDescriptor* mTRST;
};
#endif //SBW_SIMULATION_DATA_GENERATOR
