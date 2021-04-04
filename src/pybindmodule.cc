#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;

void
init_module();

std::vector<std::vector<double>>
calculate_infection(const int duration,
                    const int susceptible_max_size,
                    const int i0active,
                    const int i0recovered,
                    const int samples,
                    const int max_transmission_day,
                    const int max_in_quarantine,
                    const double gamma,
                    const double percentage_in_quarantine,
                    const bool apply_vaccine = false,
                    const double vaccinated_share = 0.0,
                    const double vaccine_efficacy = 0.0);

PYBIND11_MODULE(socnet, m)
{
    m.doc() =
      "calculate_infection implemented in C++ - v3.0"; // optional module
                                                       // docstring

    m.def("calculate_infection",
          &calculate_infection,
          "Simulate the Social Network Model for SIRE dynamics.\n"
          "calculate_infection(const int duration,\n"
          "                    const int susceptible_max_size,\n"
          "                    const int i0active,\n"
          "                    const int i0recovered,\n"
          "                    const int samples,\n"
          "                    const int max_transmission_day,\n"
          "                    const int max_in_quarantine,\n"
          "                    const double gamma,\n"
          "                    const double percentage_in_quarantine,\n"
          "                    const bool apply_vaccine = false,\n"
          "                    const double vaccinated_share = 0.0,\n"
          "                    const double vaccine_efficacy = 0.0);\n"
          "\n"
          " ret[0]  = infected_stat_mean\n"
          " ret[1]  = infected_stat_m2\n"
          " ret[2]  = infected_stat_count\n"
          " ret[3]  = susceptible_stat_mean\n"
          " ret[4]  = susceptible_stat_m2\n"
          " ret[5]  = susceptible_stat_count\n"
          " ret[6]  = cases_stat_mean\n"
          " ret[7]  = cases_stat_m2\n"
          " ret[8]  = cases_stat_count\n"
          " ret[9]  = r_0_stat_mean\n"
          " ret[10] = r_0_stat_m2\n"
          " ret[11] = r_0_stat_count\n"

    );
}