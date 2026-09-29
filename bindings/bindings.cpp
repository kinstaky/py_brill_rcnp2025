#include <nanobind/nanobind.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/string.h>
#include <tuple>
#include <optional>

#include "cpp/t0_match_viewer.h"

namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(_bindings, m) {
	m.doc() = "Python extensions for brill_rcnp2025";
	m.def(
		"get_t0_pid_path",
		&brill::GetT0CalibratedPidPath,
		"config_path"_a,
		"run"_a,
		"end_run"_a,
		"This function returns T0 calibrated PID path."
	);
	m.def(
		"generate_t0_pid",
		&brill::GenerateT0CalibratedPid,
		"config_path"_a,
		"run"_a,
		"end_run"_a,
		"This function generate T0 calibrated PID."
	);
}