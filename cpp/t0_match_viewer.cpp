#include "t0_match_viewer.h"

#include <TChain.h>
#include <TFile.h>
#include <TH2F.h>

#include "include/config.h"
#include "include/event/t0/dssd_match_event.h"
#include "include/event/gagg_event.h"
#include "include/t0_utils.h"
#include "include/t0/dssd.h"
#include "include/utils.h"

namespace brill {

std::string GetT0CalibratedPidPath(std::string config_path, int run, int end_run) {
	AppConfig config;
	if (config.Load(config_path)) return "";
	TString path = TString::Format(
		"%s/t0_cali_pid_%04d_%04d.root",
		JoinPath(config.root.workspace, config.paths.estimate).c_str(),
		run,
		end_run
	);
	return std::string(path.Data());
}

int GenerateT0CalibratedPid(std::string config_path, int run, int end_run) {
	AppConfig config;
	if (config.Load(config_path)) return -1;

	const std::string ingot_dir = JoinPath(config.root.workspace, config.paths.ingot);
	const std::string match_dir = JoinPath(config.root.workspace, config.paths.match);
	const std::string cali_dir = JoinPath(config.root.workspace, config.paths.calibration);

	TChain d1_chain("tree");
	TChain d2_chain("tree");
	TChain gagg_chain("tree");
	int added_runs = 0;
	for (int current_run = run; current_run <= end_run; ++current_run) {
		if (config.IsJumpRun(current_run)) continue;
		++added_runs;
		d1_chain.Add(TString::Format(
			"%s/t0d1_%04d.root",
			match_dir.c_str(),
			current_run
		));
		d2_chain.Add(TString::Format(
			"%s/t0d2_%04d.root",
			match_dir.c_str(),
			current_run
		));
	}
	if (added_runs == 0) {
		std::cout << "No runs to process after applying jump_run.\n";
		return 0;
	}
	d1_chain.AddFriend(&d2_chain, "d2");

	DssdMatchEvent d1_event;
	DssdMatchEvent d2_event;
	GaggEvent gagg_event;
	SetupInput(&d1_chain, d1_event);
	SetupInput(&d1_chain, d2_event, "d2.");
	SetupInput(&d1_chain, gagg_event, "gagg.");

	CalibrationParameters t0_cali(2);
	t0_cali.Read(cali_dir + "/t0.txt");

	// calibration parameters
	t0::GAGGCalibrationParameters cali_param_a(25);
	TString cali_param_path_a = TString::Format(
		"%s/gagg_layer1_a_Be.txt",
		cali_dir.c_str()
	);
	cali_param_a.Read(cali_param_path_a.Data());
	t0::GAGGCalibrationParameters cali_param_b(25);
	TString cali_param_path_b = TString::Format(
		"%s/gagg_layer1_b_Be.txt",
		cali_dir.c_str()
	);
	cali_param_b.Read(cali_param_path_b.Data());

	TString output_path = TString::Format(
		"%s/t0_cali_pid_%04d_%04d.root",
		JoinPath(config.root.workspace, config.paths.estimate).c_str(),
		run,
		end_run
	);

	TFile opf(output_path, "recreate");
	TH2F d1d2_pid("d1d2", "D1-D2 PID", 1000, 0.0, 500.0, 1000, 0.0, 500.0);
	TH2F d2_gagg_pid("d2gagg", "D2-GAGG PID", 1000, 0.0, 500.0, 1000, 0.0, 300.0);

	for (long long entry = 0; entry < d1_chain.GetEntriesFast(); ++entry) {
		d1_chain.GetEntry(entry);
		for (int i = 0; i < d1_event.num; ++i) {
			double d1_energy = t0_cali.p0[0] + t0_cali.p1[0] * d1_event.energy[i];
			for (int j = 0; j < d2_event.num; ++j) {
				double d2_energy = t0_cali.p0[1] + t0_cali.p1[1] * d2_event.energy[j];
				if (!t0::IsInTrackWindow(
					d1_event.front_strip[i],
					d1_event.back_strip[i],
					d2_event.front_strip[j],
					d2_event.back_strip[j]
				)) continue;
				d1d2_pid.Fill(d2_energy, d1_energy);
			}
		}
		for (int j = 0; j < d2_event.num; ++j) {
			double d2_energy = t0_cali.p0[1] + t0_cali.p1[1] * d2_event.energy[j];
			for (int k = 0; k < gagg_event.num; ++k) {
				if (gagg_event.index[k] >= 24) continue;
				if (!t0::IsInTrackWindow(
					d2_event.front_strip[j],
					d2_event.back_strip[j],
					gagg_event.index[k]
				)) continue;
				double gagg_energy = d1_event.run < 1079
					? cali_param_a.CaliEnergy(gagg_event.index[k], gagg_event.amplitude[k])
					: cali_param_b.CaliEnergy(gagg_event.index[k], gagg_event.amplitude[k]);
				d2_gagg_pid.Fill(gagg_energy, d2_energy);
			}
		}
	}

	opf.cd();
	d1d2_pid.Write();
	d2_gagg_pid.Write();
	opf.Close();

	return 0;
}


// T0MatchViewer::T0MatchViewer(std::string config_path, int run) : run_(run) {
// 	config_.Load(config_path);
// }


// std::tuple<int, int, int> T0MatchViewer::Meta(
// 	int entry
// ) {
// 	ipt_->GetEntry(entry);
// 	return {d1_event_.num, d2_event_.num, gagg_event_.num};
// }

// std::tuple<double, double, std::optional<double>> calibrate_t0_energy(
// 	const std::string &workspace,
// 	int run,
// 	int entry,
// 	int d1_index,
// 	int d2_index,
// 	int gagg_index=-1
// ) {
// 	// load calibration parameters
// 	CalibrationParameters t0_cali(2);
// 	if (t0_cali.Read(cali_dir + "/t0.txt")) {
// 		std::cerr << "Error: Failed to read t0 calibration parameters.\n";
// 		return -1;
// 	}
// 	brill::t0::GAGGCalibrationParameters gagg_cali(25);
// 	TString gagg_cali_path = TString::Format(
// 		"%s/calibration/gagg_layer1_%c_Be.txt",
// 		workspace.c_str(),
// 		run < 1079 ? 'a' : 'b'
// 	);
// 	if (gagg_cali.Read(gagg_cali_path.Data())) {
// 		std::cerr << "Error: Failed to read gagg calibration parameters.\n";
// 		return -1;
// 	}

// 	TChain chain1("tree");
// 	chain1.Add(TString::Format(
// 		"%s/match/t0d1_%04d.root",
// 		workspace.c_str(),
// 		run
// 	));
// 	TChain chain2("tree");
// 	chain2.Add(TString::Format(
// 		"%s/match/t0d2_%04d.root",
// 		workspace.c_str(),
// 		run
// 	));
// 	TChain chain_gagg("tree");
// 	chain_gagg.Add(TString::Format(
// 		"%s/ingot/gagg_%04d.root",
// 		workspace.c_str(),
// 		run
// 	));
// 	chain1.AddFriend(&chain2, "d2");
// 	chain1.AddFriend(&chain_gagg, "gagg");

// 	brill::DssdMatchEvent d1_event;
// 	brill::DssdMatchEvent d2_event;
// 	brill::GaggEvent gagg_event;
// 	brill::SetupInput(&chain1, d1_event);
// 	brill::SetupInput(&chain1, d2_event, "d2.");
// 	brill::SetupInput(&chain1, gagg_event, "gagg.");

// 	chain1.GetEntry(entry);
// 	double d1_energy = t0_cali.p0[0] + t0_cali.p1[0] * d1_event.energy[d1_index];
// 	double d2_energy = t0_cali.p0[1] + t0_cali.p1[1] * d2_event.energy[d2_index];
// 	std::optional<double> gagg_energy = gagg_index != -1
// 		? gagg_cali.CaliEnergy(gagg_event.index[gagg_index], gagg_event.energy[gagg_index]);
// 		: std::optional<double>::None;
// 	return {d1_energy, d2_energy, gagg_energy};
// }
}