#include "SbwAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "SbwAnalyzer.h"
#include "SbwAnalyzerSettings.h"
#include <iostream>
#include <sstream>
#include <cstdio>

#pragma warning(disable: 4996) //warning C4996: 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead.

const char *JtagStateStr[] = {
    "Reset",
    "Idle",
    
    "SelectDR",
    "CaptureDR",
    "ShiftDR",
    "Exit1DR",
    "PauseDR",
    "Exit2DR",
    "UpdateDR",

    "SelectIR",
    "CaptureIR",
    "ShiftIR",
    "Exit1IR",
    "PauseIR",
    "Exit2IR",
    "UpdateIR"
};

const struct {
    unsigned char ir;
    const char *name;
} MSPIRValues[] = {
    {0x90, "IR_EMEX_DATA_EXCHANGE"},
    {0x50, "IR_EMEX_READ_TRIGGER"},
    {0xD0, "IR_EMEX_READ_CONTROL"},
    {0x30, "IR_EMEX_WRITE_CONTROL"},
    {0xB0, "IR_EMEX_DATA_EXCHANGE32"},

    {0x54, "IR_TEST_REG"},
    {0xF4, "IR_TEST_3V_REG"},

    {0x81, "IR_ADDR_HIGH_BYTE"},
    {0x41, "IR_ADDR_LOW_BYTE"},
    {0xC1, "IR_ADDR_16BIT"},
    {0x21, "IR_ADDR_CAPTURE"},
    {0xA1, "IR_DATA_TO_ADDR"},
    {0x61, "IR_CAPTURE_CPU_REG"},
    {0xE1, "IR_DEVICE_ID"},

    {0x82, "IR_DATA_16BIT"},
    {0x42, "IR_DATA_CAPTURE"},
    {0xC2, "IR_DATA_QUICK"},
    {0x22, "IR_DATA_PSA"},
    {0xA2, "IR_DATA_16BIT_OPT"},
    {0x62, "IR_SHIFT_OUT_PSA"},
    {0xE2, "IR_DTA"},

    {0x90, "IR_BP_CNTL_16BIT"},
    {0x50, "IR_BP_CNTL_CAPTURE"},
    {0xD0, "IR_BP1_16BIT"},
    {0x30, "IR_BP1_CAPTURE"},
    {0xB0, "IR_BP2_16BIT"},
    {0x70, "IR_BP2_CAPTURE"},

    {0x98, "IR_FLASH_16BIT_UPDATE"},
    {0x58, "IR_FLASH_CAPTURE"},
    {0xD8, "IR_FLASH_16BIT_IN"},
    {0x38, "IR_FLASH_UPDATE"},

    {0x88, "IR_CNTRL_SIG_HIGH_BYTE"},
    {0x48, "IR_CNTRL_SIG_LOW_BYTE"},
    {0xC8, "IR_CNTRL_SIG_16BIT"},
    {0x28, "IR_CNTRL_SIG_CAPTURE"},
    {0xA8, "IR_CNTRL_SIG_RELEASE"},
    {0xE8, "IR_COREIP_ID"},
    {0x46, "IR_JSTATE_ID"},

    {0x84, "IR_CNTRL"},
    {0x44, "IR_PREPARE_BLOW"},
    {0x24, "IR_EX_BLOW"},

    {0x94, "IR_CONFIG_FUSES"},

    {0x8C, "IR_DUAL_8BIT"},
    {0x4C, "IR_DUAL_CAPTURE"},
    {0xCC, "IR_SELECT_MAIN"},
    {0x2C, "IR_SELECT_ESP"},

    {0xFF, "IR_BYPASS"},

    {0x9A, "IR_ACCEPT_KEY"},

    {0x86, "IR_JMB_EXCHANGE"},
    {0x11, "IR_JMB_WRITE_32BIT_MODE"},
    {0x26, "IR_TDO_EVENT"},
    {0xA6, "IR_TDO_EVENT_CTL"},
    {0, NULL}
};

SbwAnalyzerResults::SbwAnalyzerResults(SbwAnalyzer* analyzer, SbwAnalyzerSettings* settings)
:	AnalyzerResults(),
	mSettings(settings),
	mAnalyzer(analyzer)
{
}

SbwAnalyzerResults::~SbwAnalyzerResults()
{
}

void SbwAnalyzerResults::GenerateBubbleText(U64 frame_index, Channel& channel, DisplayBase display_base)
{
    char tdi[32], tdo[32], buf[64];

	ClearResultStrings();
	Frame frame = GetFrame(frame_index);

    if (channel == mSettings->mTCKChannel) {
		AddResultString(JtagStateStr[frame.mFlags & 0x0f]);
	} else if ((frame.mFlags & 0xf) == JtagShiftDR) {
		if (channel == mSettings->mTDIOChannel) {
            AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 0, tdi, 32);
			AnalyzerHelpers::GetNumberString(frame.mData2, display_base, 0, tdo, 32);
            sprintf(buf, "%s / %s", tdi, tdo);
			AddResultString(buf);
        }
    } else if ((frame.mFlags & 0xf) == JtagShiftIR) {
        int idx;
        for (idx=0; MSPIRValues[idx].name; idx++) {
            if (MSPIRValues[idx].ir == frame.mData1) {
                AddResultString(MSPIRValues[idx].name);
                return;
            }
        }

        AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 0, tdi, 32);
        AnalyzerHelpers::GetNumberString(frame.mData2, display_base, 0, tdo, 32);
        sprintf(buf, "%s / %s", tdi, tdo);
        AddResultString(buf);
    }
}

void SbwAnalyzerResults::GenerateExportFile(const char* file, DisplayBase display_base, U32 /*export_type_user_id*/)
{
	std::stringstream ss;
	void* f = AnalyzerHelpers::StartFile(file);

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	ss << "Time [s],Packet ID,State,TDI,TDO" << std::endl;

	U64 num_frames = GetNumFrames();
	for (U32 i=0; i < num_frames; i++) {
		Frame frame = GetFrame(i);

        if ((frame.mFlags & 0xf) != JtagShiftDR && (frame.mFlags & 0xf) != JtagShiftIR) {
            continue;
        }

		char time_str[128], tdi_str[128], tdo_str[128];

		AnalyzerHelpers::GetTimeString(frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128);

		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 0, tdi_str, 128);
		AnalyzerHelpers::GetNumberString(frame.mData2, display_base, 0, tdo_str, 128);

		U64 packet_id = GetPacketContainingFrameSequential(i); 
		if (packet_id != INVALID_RESULT_INDEX) {
			ss << time_str << "," << packet_id << "," << JtagStateStr[frame.mFlags & 0x0f] << "," << tdi_str << "," << tdo_str << std::endl;
        } else {
			ss << time_str << ",," << JtagStateStr[frame.mFlags & 0x0f] << "," << tdi_str << "," << tdo_str << std::endl;
        }
	
		AnalyzerHelpers::AppendToFile((U8 *) ss.str().c_str(), ss.str().length(), f);
		ss.str(std::string());
							
		if (UpdateExportProgressAndCheckForCancel(i, num_frames) == true) {
			AnalyzerHelpers::EndFile(f);
			return;
		}
	}

	UpdateExportProgressAndCheckForCancel(num_frames, num_frames);
	AnalyzerHelpers::EndFile(f);
}

void SbwAnalyzerResults::GenerateFrameTabularText(U64 /*frame_index*/, DisplayBase /*display_base*/)
{
	ClearResultStrings();
	AddResultString("not supported");
}

void SbwAnalyzerResults::GeneratePacketTabularText(U64 /*packet_id*/, DisplayBase /*display_base*/)
{
	ClearResultStrings();
	AddResultString("not supported");
}

void SbwAnalyzerResults::GenerateTransactionTabularText(U64 /*transaction_id*/, DisplayBase /*display_base*/)
{
	ClearResultStrings();
	AddResultString("not supported");
}
