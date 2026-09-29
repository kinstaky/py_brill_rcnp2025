#pragma once

#include <string>
#include <tuple>
#include <optional>
#include <memory>

#include <TFile.h>
#include <TTree.h>

#include "include/event/t0/dssd_match_event.h"
#include "include/event/gagg_event.h"
#include "include/config.h"
#include "include/t0/dssd.h"
#include "include/t0_utils.h"

namespace brill {

std::string GetT0CalibratedPidPath(std::string config_path, int run, int end_run);
int GenerateT0CalibratedPid(std::string config_path, int run, int end_run);

class T0MatchViewer {
public:
	T0MatchViewer(const std::string &config_path, int run);
	~T0MatchViewer();

	int GetEntries() const;
	std::tuple<int, int, int> Meta(int entry);
	std::tuple<std::optional<double>, std::optional<double>, std::optional<double>> CalibratedEnergy(
		int entry,
		int d1_index=-1,
		int d2_index=-1,
		int gagg_index=-1
	);
private:
	int run_;
	AppConfig config_;
	CalibrationParameters t0_cali_;
	t0::GAGGCalibrationParameters gagg_cali_;
	DssdMatchEvent d1_event_;
	DssdMatchEvent d2_event_;
	GaggEvent gagg_event_;
	TFile *ipf_;
	TTree *ipt_;
};

}