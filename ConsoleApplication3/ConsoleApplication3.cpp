
//#include <iostream>
//#include <vector>
//#include <random>
//#include <algorithm>
//#include <iomanip>
//#include <locale>
//#include <fstream>
//
//using namespace std;
//
//
//struct Agent {
//    float money;
//    float productivity;
//    float reputation;
//
//    float fraud_tendency;   // Crime chance 
//    bool is_fraudulent;
//
//    float last_money;       // learning memory
//};
//
//float clampf(float value, float low, float high) {
//    if (value < low) return low;
//    if (value > high) return high;
//    return value;
//}
//
//void updateReputation(Agent& a, float produced) {
//    const float expected = 1.0;
//    const float alpha = 0.05;
//    const float decay = 0.99;
//
//    float delta = (produced - expected) / expected;
//    a.reputation += alpha * delta;
//    a.reputation *= decay;
//    a.reputation = clampf(a.reputation, 0.0f, 1.0f);
//
//}
//
//void learn(Agent& a) {
//    const float learning_rate = 0.02f;
//    const float regret_rate = 0.05f;
//
//    float payoff = a.money - a.last_money;
//
//    if (a.is_fraudulent) {
//        if (payoff > 0) {
//            a.fraud_tendency += learning_rate * payoff;
//        }
//        else {
//            a.fraud_tendency -= regret_rate * (-payoff);
//        }
//    }
//    else {
//        if (payoff < 0) {
//            a.fraud_tendency += learning_rate * (-payoff);
//        }
//        else {
//            a.fraud_tendency *= 0.999f; // slow decay toward honesty
//        }
//    }
//
//    a.fraud_tendency = clampf(a.fraud_tendency, 0.0f, 1.0f);
//}
//
//int main() {
//    const uint64_t NUM_AGENTS = 100;
//    const uint64_t TURNS = 200;
//    float price = 1.00;
//
//    ofstream rep_file("reputation_over_time.csv");
//    rep_file << "Turn,AvgReputation,FraudsterReputation,HonestReputation,FraudsterCount\n";
//
//    mt19937 rng(42);
//    uniform_real_distribution<float> dist(0.5, 1.5);
//    vector<Agent> agents;
//
//    uniform_real_distribution<float> fraud_dist(0.0f, 1.0f);
//
//
//    for (uint64_t i = 0; i < NUM_AGENTS; i++) {                 //initialisation
//        float tendency = fraud_dist(rng);
//
//        agents.push_back({
//            10.0f,
//            dist(rng),
//            0.5f,
//            tendency,
//            false
//            });
//    }
//
//    for (auto& j : agents) {
//        j.last_money = j.money;
//
//    }
//
//    for (uint64_t turn = 0; turn < TURNS; turn++) {
//        float total_supply = 0.0;
//        float total_demand = NUM_AGENTS * 1.0;
//        float total_rep = 0.0;
//
//        for (auto& j : agents) {   //decides fraud
//            j.is_fraudulent = false;
//
//            if (fraud_dist(rng) < j.fraud_tendency) {
//                j.is_fraudulent = true;
//                total_rep += j.reputation ;  // Short term gain for realism
//            }
//            else {
//                total_rep += j.reputation;
//            }
//        }
//
//        for (auto& j : agents) {  //production
//            float produced = j.productivity;
//            total_supply += produced;
//            total_rep += j.reputation;
//
//            j.money += produced * price;
//        }
//
//        if (total_demand > total_supply)  //supply and demand price changes
//            price *= 1.05;
//        else
//            price *= 0.95;
//
//
//        for (auto& j : agents) {
//            float produced = j.productivity;
//
//            float total_effective_rep = 0.0f;
//            for (auto& j : agents) {
//                float effective = j.is_fraudulent ? j.reputation * 1.5f : j.reputation;
//                total_effective_rep += effective;
//            }
//
//            float share = (total_rep > 0)
//                ? total_effective_rep / total_rep
//                : 1.0f / NUM_AGENTS;
//
//            float market_buy = share * total_demand;
//            float sold = min(produced, market_buy);
//
//            j.money += sold * price;
//
//            updateReputation(j, produced);
//
//            const float audit_rate = 0.05f; // strength of regulation
//
//            if (j.is_fraudulent && fraud_dist(rng) < audit_rate) { // is caught
//                j.reputation *= 0.2f; //reputation hit
//                j.money *= 0.9f; // fine incurred
//            }
//
//            float total_rep = 0.0f;
//            float fraud_rep = 0.0f;
//            float honest_rep = 0.0f;
//
//            float avg_fraud_tendency = 0.0f;
//
//            int fraud_count = 0;
//            int honest_count = 0;
//
//            for (auto& a : agents) {
//                total_rep += a.reputation;
//                avg_fraud_tendency += a.fraud_tendency;
//
//                if (a.is_fraudulent) {
//                    fraud_rep += a.reputation;
//                    fraud_count++;
//                }
//                else {
//                    honest_rep += a.reputation;
//                    honest_count++;
//                }
//            }
//
//            float avg_rep = total_rep / NUM_AGENTS;
//            avg_fraud_tendency /= NUM_AGENTS;
//            float avg_fraud_rep = fraud_count > 0 ? fraud_rep / fraud_count : 0.0f;
//            float avg_honest_rep = honest_count > 0 ? honest_rep / honest_count : 0.0f;
//
//            if (turn % 20 == 0) {
//
//                rep_file << turn << ","
//                    << avg_rep << ","
//                    << avg_fraud_rep << ","
//                    << avg_honest_rep << ","
//                    << fraud_count << ","
//                    << avg_fraud_tendency << "\n";
//            }
//        }
//
//        for (auto& a : agents) {
//            learn(a);
//            a.last_money = a.money;
//        }
//
//        for (auto& j : agents) {
//            float desired = 1.0;            //desired to affordability check.
//            float affordable = j.money / price;
//            float bought = min(desired, affordable);
//            j.money -= bought * price;
//        }
//
//        float total_money = 0;
//        for (auto& j : agents)
//            total_money += j.money;
//
//        if (turn % 20 == 0) {
//            //cout << setprecision(2) << price; - was trying to round to 2 decimal places and wasnt working will resolve later
//            // weird issue where the debug console didnt like £ idk what the issue is will also resolve this later
//            cout << fixed << setprecision(2);
//            cout << "Turn " << turn
//                << " | Price: GBP " << price
//                << " | Avg Money: GBP " << total_money / NUM_AGENTS
//                << endl;
//
//            for (uint64_t i = 0; i < NUM_AGENTS; i++)
//            {
//                if (agents[i].reputation < 1)
//                {
//                    cout << "Agent Number : " << i
//                        << " | Reputation :" << agents[i].reputation
//                        << endl;
//                }
//            }
//        }
//        
//    }
//    rep_file.close();
//}

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <cstdint>

using namespace std;

struct Agent {
    double money;
    double productivity;
    double reputation;
    double fraud_tendency;
    bool is_fraudulent;
    double last_money;
};

struct Result {
    vector<double> fraud_series;
    vector<double> price_series;
};

double clampd(double value, double low, double high) {
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

void updateReputation(Agent& a, double produced) {
    const double expected = 1.0;
    const double alpha = 0.05;
    const double decay = 0.99;

    double delta = (produced - expected) / expected;
    a.reputation += alpha * delta;
    a.reputation *= decay;
    a.reputation = clampd(a.reputation, 0.0, 1.0);
}

void learn(Agent& a) {
    const double learning_rate = 0.02;
    const double regret_rate = 0.05;

    double payoff = a.money - a.last_money;

    if (a.is_fraudulent) {
        if (payoff > 0)
            a.fraud_tendency += learning_rate * payoff;
        else
            a.fraud_tendency -= regret_rate * (-payoff);
    }
    else {
        if (payoff < 0)
            a.fraud_tendency += learning_rate * (-payoff);
        else
            a.fraud_tendency *= 0.999;
    }

    a.fraud_tendency = clampd(a.fraud_tendency, 0.0, 1.0);
}

Result simulate(bool use_reputation, uint64_t seed) {

    const uint64_t NUM_AGENTS = 100;
    const uint64_t TURNS = 200;
    const double audit_rate = 0.05;

    mt19937 rng(seed);
    uniform_real_distribution<double> productivity_dist(0.5, 1.5);
    uniform_real_distribution<double> fraud_dist(0.0, 1.0);

    vector<Agent> agents(NUM_AGENTS);
    for (uint64_t i = 0; i < NUM_AGENTS; i++) {
        agents[i] = { 10.0, productivity_dist(rng), 0.5, fraud_dist(rng), false, 10.0 };
    }

    double price = 1.0;
    vector<double> fraud_series(TURNS, 0.0);
    vector<double> price_series(TURNS, 0.0);

    for (uint64_t turn = 0; turn < TURNS; turn++) {

        double total_supply = 0.0;
        double total_demand = NUM_AGENTS * 1.0;

        // Decide fraud
        for (auto& a : agents) {
            a.is_fraudulent = (fraud_dist(rng) < a.fraud_tendency);
        }

        // Count fraud this turn
        uint64_t fraud_count_turn = 0;
        for (auto& a : agents)
            if (a.is_fraudulent)
                fraud_count_turn++;

        fraud_series[turn] = fraud_count_turn / static_cast<double>(NUM_AGENTS);

        // Production (fraud reduces productivity)
        for (auto& a : agents) {
            double effective_productivity = a.productivity;
            if (a.is_fraudulent) {
                effective_productivity *= 0.7; // Fraud reduces output by 30%
            }
            total_supply += effective_productivity;
            a.money += effective_productivity * price;
        }

        // Price update (smoothed)
        double excess = (total_demand - total_supply) / total_supply;
        price *= (1.0 + 0.1 * excess);
        price = max(price, 0.01);
        price_series[turn] = price;

        // Total effective reputation
        double total_effective_rep = 0.0;
        for (auto& a : agents) {
            double effective = use_reputation ? (a.is_fraudulent ? a.reputation * 1.5 : a.reputation) : 1.0;
            total_effective_rep += effective;
        }

        // Market allocation
        for (auto& a : agents) {
            double effective = use_reputation ? (a.is_fraudulent ? a.reputation * 1.5 : a.reputation) : 1.0;
            double share = effective / total_effective_rep;
            double market_buy = share * total_demand;
            double sold = min(a.productivity, market_buy);
            a.money += sold * price;

            updateReputation(a, a.productivity);

            // Audit
            if (a.is_fraudulent && fraud_dist(rng) < audit_rate) {
                a.reputation *= 0.2;
                a.money *= 0.9;
            }
        }

        // Learning
        for (auto& a : agents) {
            learn(a);
            a.last_money = a.money;
        }

        // Consumption
        for (auto& a : agents) {
            double desired = 1.0;
            double affordable = a.money / price;
            double bought = min(desired, affordable);
            a.money -= bought * price;
        }
    }

    Result r;
    r.fraud_series = fraud_series;
    r.price_series = price_series;
    return r;
}

int main() {

    const uint64_t N_RUNS = 100;
    const uint64_t base_seed = 42;
    const uint64_t TURNS = 200;

    vector<Result> results_rep(N_RUNS);
    vector<Result> results_no_rep(N_RUNS);

    // Run simulations
    for (uint64_t run = 0; run < N_RUNS; run++) {
        uint64_t seed = base_seed + run;
        results_rep[run] = simulate(true, seed);
        results_no_rep[run] = simulate(false, seed);
    }

    // Average across runs
    vector<double> avg_fraud_rep(TURNS, 0.0), avg_price_rep(TURNS, 0.0);
    vector<double> avg_fraud_no_rep(TURNS, 0.0), avg_price_no_rep(TURNS, 0.0);

    for (uint64_t run = 0; run < N_RUNS; run++) {
        for (uint64_t t = 0; t < TURNS; t++) {
            avg_fraud_rep[t] += results_rep[run].fraud_series[t];
            avg_price_rep[t] += results_rep[run].price_series[t];

            avg_fraud_no_rep[t] += results_no_rep[run].fraud_series[t];
            avg_price_no_rep[t] += results_no_rep[run].price_series[t];
        }
    }

    for (uint64_t t = 0; t < TURNS; t++) {
        avg_fraud_rep[t] /= static_cast<double>(N_RUNS);
        avg_price_rep[t] /= static_cast<double>(N_RUNS);
        avg_fraud_no_rep[t] /= static_cast<double>(N_RUNS);
        avg_price_no_rep[t] /= static_cast<double>(N_RUNS);
    }

    // Export CSV ready for Excel plotting
    ofstream out("ReputationExperiment.csv");
    out << "Turn,Rep_Fraud,NoRep_Fraud,Rep_Price,NoRep_Price\n";

    for (uint64_t t = 0; t < TURNS; t++) {
        out << t << ","
            << avg_fraud_rep[t] << ","
            << avg_fraud_no_rep[t] << ","
            << avg_price_rep[t] << ","
            << avg_price_no_rep[t] << "\n";
    }

    out.close();

    cout << "CSV file 'ReputationExperiment.csv' generated successfully.\n";
    return 0;
}

