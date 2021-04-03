#ifndef POPULATION_HPP
#define POPULATION_HPP

#include <memory>
#include <random>
#include <vector>

constexpr uint8_t fACTIVE = 0x01;
constexpr uint8_t fQUARANTINE = 0x01 << 1;
constexpr uint32_t subject_Adam = 0;

class Subject
{
  private:
  public:
    uint8_t flags;
    uint8_t days_of_infection;
    uint32_t parent;
    uint16_t contamination_day;
    uint8_t decendants;

    inline bool is_active() { return this->flags & fACTIVE; }
    inline void set_active() { this->flags ^= fACTIVE; }
    inline void clear_active() { this->flags &= ~fACTIVE; }
    inline bool is_quarantined() { return this->flags & fQUARANTINE; }
    inline void set_quarantined() { this->flags ^= fQUARANTINE; }
    inline void clear_quarantined() { this->flags &= ~fQUARANTINE; }
    inline void set_active_and_quarantine(bool a, bool q)
    {
        this->flags = uint8_t(a) | (uint8_t(q) << 1);
    }
    Subject(const int doi = 0,
            const int p = subject_Adam,
            const int c = 0,
            const bool a = false,
            const bool q = false)
      : days_of_infection(doi)
      , parent(p)
      , contamination_day(c)
      , decendants(0)
    {
        this->flags = uint8_t(a) | (uint8_t(q) << 1);
    }
    Subject(const bool a, const bool q)
      : days_of_infection(0)
      , parent(subject_Adam)
      , contamination_day(0)
      , decendants(0)
    {
        this->flags = uint8_t(a) | (uint8_t(q) << 1);
    }
};

class Population
{
    int first_ind;
    std::mt19937_64 my_gen;
    std::uniform_real_distribution<> dis;
    std::uniform_int_distribution<> pop_dis;
    std::uniform_int_distribution<> day_dis;
    std::vector<Subject> population;

  public:
    Population(const int expected_size = 1000,
               const int seed = 1234,
               const int max_transmission_day = 14)
      : first_ind(subject_Adam)
      , my_gen(seed)
      , dis(0.0, 1.0)
      , pop_dis(0, expected_size)
      , day_dis(1, max_transmission_day - 1)
    {
        population.reserve(expected_size);
        first_ind = subject_Adam;
    }
    ~Population() { reset_population(); }
    Subject& operator[](const int index) { return population[index]; }

    double rnd_dis() { return this->dis(my_gen); }
    int rnd_pop_dis() { return this->pop_dis(my_gen); }
    int rnd_day_dis() { return this->day_dis(my_gen); }

    void new_subject(const int day,
                     const int parent,
                     const int cDay,
                     const bool active,
                     const bool quarantine)
    {
        population.push_back(Subject(day, parent, cDay, active, quarantine));
    }
    void seed_subject(const bool active, const bool quarantine)
    {
        population.push_back(Subject(active, quarantine));
    }
    void reset_population() { population.clear(); }
    void seed_infected(const int i0active,
                       const int i0recovered,
                       const double percentage);
    void seed_infected(const std::vector<int>& i0active,
                       const std::vector<int>& i0recovered,
                       const double percentage);
    unsigned int size() const { return population.size(); }
    int first_subject() const { return first_ind; }
    void move_first(const int id) { first_ind = id; }
};

#endif // POPULATION_HPP