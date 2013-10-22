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
