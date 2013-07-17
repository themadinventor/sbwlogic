#ifndef SBW_ANALYZER_RESULTS
#define SBW_ANALYZER_RESULTS

#include <AnalyzerResults.h>

//#define JTAG_ERROR_FLAG ( 1 << 0 )

class SbwAnalyzer;
class SbwAnalyzerSettings;

class SbwAnalyzerResults : public AnalyzerResults
{
public:
	SbwAnalyzerResults(SbwAnalyzer* analyzer, SbwAnalyzerSettings* settings);
	virtual ~SbwAnalyzerResults();

	virtual void GenerateBubbleText(U64 frame_index, Channel& channel, DisplayBase display_base);
	virtual void GenerateExportFile(const char* file, DisplayBase display_base, U32 export_type_user_id);

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base);
	virtual void GeneratePacketTabularText(U64 packet_id, DisplayBase display_base);
	virtual void GenerateTransactionTabularText(U64 transaction_id, DisplayBase display_base);

protected: //functions

protected:  //vars
	SbwAnalyzerSettings* mSettings;
	SbwAnalyzer* mAnalyzer;
};

#endif //SBW_ANALYZER_RESULTS
