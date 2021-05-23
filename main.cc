#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <random>

struct DRAW_RESULTS
{
    long delta;
    std::map<std::string, uint> counts;
};

class ModifierCard
{
public:
    ModifierCard(bool rolling, bool shuffle, std::string type, int delta)
        : rolling_(rolling),
          shuffle_(shuffle),
          type_(type),
          delta_(delta) {}

    bool is_rolling() const { return rolling_; }
    bool is_shuffle() const { return shuffle_; }
    const std::string &type() const { return type_; }
    int delta() const { return delta_; }

private:
    bool rolling_;
    bool shuffle_;
    std::string type_;
    int delta_;
};

class CardFactory
{
public:
    CardFactory() {}
    ModifierCard *normal_card(int delta)
    {
        return new ModifierCard(false, false, "normal", delta);
    }
    ModifierCard *miss_card()
    {
        return new ModifierCard(false, true, "miss", 0);
    }
    ModifierCard *crit_card()
    {
        return new ModifierCard(false, true, "crit", 0);
    }
};

class Deck
{
public:
    Deck() : factory_(), deck_(), pos_(0)
    {
        deck_.push_back(factory_.miss_card());
        deck_.push_back(factory_.normal_card(-2));
        for (int i = 0; i < 5; ++i)
        {
            deck_.push_back(factory_.normal_card(-1));
        }
        for (int i = 0; i < 6; ++i)
        {
            deck_.push_back(factory_.normal_card(0));
        }
        for (int i = 0; i < 5; ++i)
        {
            deck_.push_back(factory_.normal_card(1));
        }
        deck_.push_back(factory_.normal_card(2));
        deck_.push_back(factory_.crit_card());
    }

    void shuffle()
    {
        std::random_shuffle(deck_.begin(), deck_.end());
    }

    const ModifierCard *draw()
    {
        if (pos_ >= deck_.size())
        {
            shuffle();
            pos_ = 0;
        }
        const ModifierCard *card = deck_[pos_];
        ++pos_;
        return card;
    }

private:
    CardFactory factory_;
    std::vector<const ModifierCard *> deck_;
    uint pos_;
};

class DrawResult
{
public:
    DrawResult() : delta_(0), attacks_(0)
    {
        counts_ = std::map<std::string, uint>();
    }
    void handle_card(const ModifierCard *card)
    {
        if (counts_.find(card->type()) == counts_.end())
            counts_[card->type()] = 0;
        ++counts_[card->type()];
        delta_ += card->delta();
        if(!card->is_rolling())
            ++attacks_;
    }

    uint get_count(const std::string& type){
        if(counts_.find("normal") == counts_.end())
            return 0;
        return counts_[type];
    }

    double expected_count(const std::string& type){
        return get_count(type) / double(attacks_);
    }

    double expected_delta(){
        return delta_ / double(attacks_);
    }

private:
    long delta_;
    // One attack is one or more handle_cards.
    // Specifically, if multiple rolling cards are pulled, they are handled
    // but do not increment attacks_. Only non-rolling modifiers do that.
    long attacks_;
    std::map<std::string, uint> counts_;
};

class Simulation
{
public:
    Simulation(uint draws_per_scenario, uint scenarios)
        : draws_per_scenario_(draws_per_scenario),
          scenarios_(scenarios),
          deck_()
    {
        results_ = std::vector<DrawResult>(draws_per_scenario_);
    }

    void run_scenario()
    {
        for (uint i = 0; i < draws_per_scenario_; ++i)
        {
            const ModifierCard *card = deck_.draw();
            results_[i].handle_card(card);
        }
    }
    void run_sim()
    {
        for (uint i = 0; i < scenarios_; ++i)
        {
            run_scenario();
        }
        for (auto result : results_){
            printf("% 0.10f\t% 0.10f\t% 0.10f\n", result.expected_delta(), result.expected_count("crit"), result.expected_count("miss"));
        }
    }

private:
    uint draws_per_scenario_;
    uint scenarios_;
    Deck deck_;
    std::vector<DrawResult> results_;
};

int main()
{
    Simulation sim(30, 100000);
    sim.run_sim();
    return 0;
}