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

#include "SbwAnalyzerSettings.h"

#include <AnalyzerHelpers.h>
#include <sstream>
#include <cstring>

SbwAnalyzerSettings::SbwAnalyzerSettings()
:	mTDIOChannel(UNDEFINED_CHANNEL),
	mTCKChannel(UNDEFINED_CHANNEL)
{
	mTCKChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mTCKChannelInterface->SetTitleAndTooltip("SBWTCK", "Test Clock");
	mTCKChannelInterface->SetChannel(mTCKChannel);

	mTDIOChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mTDIOChannelInterface->SetTitleAndTooltip("SBWTDIO", "Test Data Input/Output");
	mTDIOChannelInterface->SetChannel(mTDIOChannel);

	AddInterface(mTCKChannelInterface.get());
	AddInterface(mTDIOChannelInterface.get());

	AddExportOption(0, "Export as text/csv file");
	AddExportExtension(0, "text", "txt");
	AddExportExtension(0, "csv", "csv");

	ClearChannels();
	AddChannel(mTCKChannel, "SBWTCK", false);
	AddChannel(mTDIOChannel, "SBWTDIO", false);
}

SbwAnalyzerSettings::~SbwAnalyzerSettings()
{
}

bool SbwAnalyzerSettings::SetSettingsFromInterfaces()
{
	Channel tck = mTCKChannelInterface->GetChannel();
	Channel tdio = mTDIOChannelInterface->GetChannel();

	std::vector<Channel> channels;
	channels.push_back(tck);
	channels.push_back(tdio);

	if (AnalyzerHelpers::DoChannelsOverlap(&channels[0], channels.size()) == true) {
		SetErrorText("Please select different channels for each input.");
		return false;
	}

	mTCKChannel = mTCKChannelInterface->GetChannel();
	mTDIOChannel = mTDIOChannelInterface->GetChannel();

	ClearChannels();
	AddChannel(mTCKChannel, "SBWTCK", mTCKChannel != UNDEFINED_CHANNEL);
	AddChannel(mTDIOChannel, "SBWTDIO", mTDIOChannel != UNDEFINED_CHANNEL);

	return true;
}

void SbwAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString(settings);

	const char* name_string;    //the first thing in the archive is the name of the protocol analyzer that the data belongs to.
	text_archive >> &name_string;
	if (strcmp(name_string, "KongoSbwAnalyzer") != 0) {
		AnalyzerHelpers::Assert( "KongoSbwAnalyzer: Provided with a settings string that doesn't belong to us;" );
	}

	text_archive >> mTCKChannel;
	text_archive >> mTDIOChannel;

	ClearChannels();
	AddChannel(mTCKChannel, "TCK", mTCKChannel != UNDEFINED_CHANNEL);
	AddChannel(mTDIOChannel, "TDIO", mTDIOChannel != UNDEFINED_CHANNEL);

	UpdateInterfacesFromSettings();
}

const char* SbwAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << "KongoSbwAnalyzer";
	text_archive <<  mTCKChannel;
	text_archive <<  mTDIOChannel;

	return SetReturnString(text_archive.GetString());
}

void SbwAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mTCKChannelInterface->SetChannel(mTCKChannel);
	mTDIOChannelInterface->SetChannel(mTDIOChannel);
}
