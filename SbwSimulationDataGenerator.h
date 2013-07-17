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
