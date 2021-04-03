#include <algorithm>
#include <cmath>
#include <future> // std::async, std::future
#include <iostream>
#include <memory>
#include <random>
#include <thread>

#include "population.hpp"
#include "statistics.hpp"

inline int
powerlaw(double gamma, double ran)
{
    return int((pow(ran, (-1.0 / gamma))) - 0.5);
}

inline int
find_first(Population& population)
{
    const unsigned int size = population.size();
    int first = size - 1;

    for (unsigned int k = 0; k < size; k++) {
        if (population[k].is_active()) {
            first = k;
            break;
        }
    }
    return first;
}

int
vaccinate(const int individuals,
          const double vaccinated_percentage,
          const double vaccine_efficacy,
          Population& p)
{
    int immune_individuals = 0;
    double real_efficacy = vaccinated_percentage * vaccine_efficacy;

    for (auto i = 0; i < individuals; i++) {
        immune_individuals += (p.rnd_dis() < real_efficacy);
    }
    return immune_individuals;
}

std::vector<std::vector<double>>
_calculate_infection_sample(const int duration,
                            const int susceptible_max_size,
                            const int i0active,
                            const int i0recovered,
                            const int max_transmission_day,
                            const int max_in_quarantine,
                            const double gamma,
                            const double percentage_in_quarantine,
                            const bool apply_vaccine,
                            const double vaccinated_share,
                            const double vaccine_efficacy,
                            const int my_seed)
{
    int S = susceptible_max_size - i0active - i0recovered;

    Population population(S, my_seed, max_transmission_day);

    MyStatistics<double> infected_stat(duration, 0.0);
    MyStatistics<double> susceptible_stat(duration, 0.0);
    MyStatistics<double> cases_stat(duration, 0.0);
    MyStatistics<double> r_0_stat(duration, 0.0);

    population.seed_infected(i0active, i0recovered, percentage_in_quarantine);

    for (int day = 0; day < duration; day++) {
        const int I = population.size();
        population.move_first(find_first(population));
        int day_cases = 0;

        infected_stat.add_value(day, double(I));
        susceptible_stat.add_value(day, double(S));

        for (int ind = population.first_subject(); ind < I; ind++) {
            auto& person = population[ind];

            if (person.is_active()) {
                if (person.days_of_infection < max_transmission_day) {
                    person.days_of_infection++;

                    int available_new_infected =
                      powerlaw(gamma, population.rnd_dis());

                    if (apply_vaccine)
                        available_new_infected -=
                          vaccinate(available_new_infected,
                                    vaccinated_share,
                                    vaccine_efficacy,
                                    population);

                    if (!available_new_infected)
                        continue;

                    if (person.is_quarantined())
                        available_new_infected =
                          std::min(max_in_quarantine - person.decendants,
                                   available_new_infected);
                    int new_infected = 0;
                    for (int ni = 0; ni < available_new_infected; ni++) {
                        // Check if the individual belongs to S, and
                        if ((population.rnd_pop_dis() < S) && (S > 0)) {
                            new_infected++;
                            day_cases++;
                            S--;
                            population.new_subject(0,
                                                   ind,
                                                   day,
                                                   true,
                                                   (population.rnd_dis() <
                                                    percentage_in_quarantine));
                        }
                    }
                    person.decendants += new_infected;
                } else {
                    person.clear_active();
                    if (population.first_subject() == ind) {
                        population.move_first(ind + 1);
                    }
                }
            }
        }
        int kp = 0, dp = 0;
        for (unsigned int ui = 0; ui < population.size(); ui++) {
            // for (auto person : population)
            auto& person = population[ui];
            if ((person.parent == subject_Adam) ||
                (person.days_of_infection < max_transmission_day))
                continue;
            kp++;
            dp += person.decendants;
        }
        if (kp)
            r_0_stat.add_value(day, double(dp) / double(kp));

        cases_stat.add_value(day, day_cases);
    }
    population.reset_population();

    std::vector<std::vector<double>> res;

    res.push_back(infected_stat.get_mean());    // 0
    res.push_back(susceptible_stat.get_mean()); // 1
    res.push_back(cases_stat.get_mean());       // 2
    res.push_back(r_0_stat.get_mean());         // 3

    return res;
}

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
                    const double vaccine_efficacy = 0.0)
{
    MyStatistics<double> infected_stat(duration, 0.0);
    MyStatistics<double> susceptible_stat(duration, 0.0);
    MyStatistics<double> cases_stat(duration, 0.0);
    MyStatistics<double> r_0_stat(duration, 0.0);

    const int div = std::thread::hardware_concurrency();

    const int work = std::max(div, samples);

    int sample_id = 0;

    for (int k = 0; k < work / div; k++) {
        std::vector<std::future<std::vector<std::vector<double>>>> fut;

        for (int i = 0; i < div; i++)
            fut.push_back(std::async(_calculate_infection_sample,
                                     duration,
                                     susceptible_max_size,
                                     i0active,
                                     i0recovered,
                                     max_transmission_day,
                                     max_in_quarantine,
                                     gamma,
                                     percentage_in_quarantine,
                                     apply_vaccine,
                                     vaccinated_share,
                                     vaccine_efficacy,
                                     sample_id++));

        for (auto& it : fut) {
            auto ret = it.get();
            for (int d = 0; d < duration; d++) {
                infected_stat.add_value(d, ret[0][d]);
                susceptible_stat.add_value(d, ret[1][d]);
                cases_stat.add_value(d, ret[2][d]);
                r_0_stat.add_value(d, ret[3][d]);
            }
        }
    }

    std::vector<std::vector<double>> res;

    res.push_back(infected_stat.get_mean());  // 0
    res.push_back(infected_stat.get_m2());    // 1
    res.push_back(infected_stat.get_count()); // 2

    res.push_back(susceptible_stat.get_mean());  // 3
    res.push_back(susceptible_stat.get_m2());    // 4
    res.push_back(susceptible_stat.get_count()); // 5

    res.push_back(cases_stat.get_mean());  // 6
    res.push_back(cases_stat.get_m2());    // 7
    res.push_back(cases_stat.get_count()); // 8

    res.push_back(r_0_stat.get_mean());  // 9
    res.push_back(r_0_stat.get_m2());    // 10
    res.push_back(r_0_stat.get_count()); // 11

    return res;
}