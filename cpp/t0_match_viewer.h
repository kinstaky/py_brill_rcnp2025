#pragma once

#include <string>
#include <tuple>
#include <optional>

#include <TTree.h>

#include "include/event/t0/dssd_match_event.h"
#include "include/event/gagg_event.h"
#include "include/config.h"

namespace brill {

std::string GetT0CalibratedPidPath(std::string config_path, int run, int end_run);
int GenerateT0CalibratedPid(std::string config_path, int run, int end_run);

// class T0MatchViewer {
// public:
// 	T0MatchViewer(int run);
// 	std::tuple<int, int, int> Meta(int entry);
// 	std::tuple<double, double, std::optional<double>> CalibratedEnergy(int entry);
// private:
// 	int run_;
// 	AppConfig config_;
// 	DssdMatchEvent d1_event_;
// 	DssdMatchEvent d2_event_;
// 	GaggEvent gagg_event_;
// 	TTree *ipt_;
// };

}