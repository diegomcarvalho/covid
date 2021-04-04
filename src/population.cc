#include "population.hpp"

void
Population::seed_infected(const int i0active,
                          const int i0recovered,
                          const double percentage)
{
    const auto recoverd = (i0recovered == 0) ? 1 : i0recovered;

    for (int i = 0; i < recoverd; i++) {
        new_subject(
          14, subject_Adam, 0, false, (this->dis(my_gen) < percentage));
    }

    for (int i = 0; i < i0active; i++) {
        new_subject(this->day_dis(my_gen),
                    subject_Adam,
                    0,
                    true,
                    (this->dis(my_gen) < percentage));
    }

    this->first_ind = i0recovered;

    return;
}

void
Population::seed_infected(const std::vector<int>& i0active,
                          const std::vector<int>& i0recovered,
                          const double percentage)
{
    for (auto& n : i0recovered) {
        for (int i = 0; i < n; i++) {
            seed_subject(false, (this->dis(my_gen) < percentage));
        }
    }

    for (auto& n : i0active) {
        for (int i = 0; i < n; i++) {
            new_subject(this->day_dis(my_gen),
                        subject_Adam,
                        0,
                        true,
                        (this->dis(my_gen) < percentage));
        }
    }

    this->first_ind = i0recovered.size();

    return;
}
