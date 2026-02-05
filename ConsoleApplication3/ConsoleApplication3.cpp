//struct Agent {
//    int id;
//    double reputation;      // public signal
//    int successfulTrades;
//    int failedTrades;
//    double wealth;
//};
//
//struct EconomyStats {
//    int totalTrades;
//    int successfulTrades;
//    double averageReputation;
//    double totalWealth;
//};
//
//void updateReputation(Agent& a, bool success) {
//    const double alpha = 0.1; // learning rate / memory decay
//
//    double feedback = success ? 1.0 : 0.0;
//    a.reputation = (1 - alpha) * a.reputation + alpha * feedback;
//}
//
//bool shouldTrade(const Agent& me, const Agent& partner) {
//    double trustThreshold = 0.6;
//    return partner.reputation >= trustThreshold;
//}
//
//bool shouldTradeProb(const Agent& partner) {
//    double p = partner.reputation;
//    return ((double)rand() / RAND_MAX) < p;
//}
//
//bool decideToCheat(const Agent& a) {
//    double shortTermGain = 10.0;
//    double futureLoss = a.reputation * 50.0;
//
//    return shortTermGain > futureLoss;
//}
//bool giveNegativeFeedback() {
//    double probabilityOfHonesty = 0.7; // feedback bias
//    return ((double)rand() / RAND_MAX) < probabilityOfHonesty;
//}
//
//int main()
//{
//    /*for (int t = 0; t < T; ++t) {
//        Agent& buyer = agents[rand() % N];
//        Agent& seller = agents[rand() % N];
//
//        if (shouldTrade(buyer, seller)) {
//            bool success = !decideToCheat(seller);
//            updateReputation(seller, success);
//        }
//    }*/
//
//    // usually people don't try to buy or sell things
//    if rand.Float64() > 0.1 {
//        return
//    }
//
//    actor.timeSinceLastTransaction++
//
//        // only buyers initiate transactions (usually buyers come to sellers, not the other way around)
//        if actor.isBuyer() {
//            // look for a seller, simulates going from shop to shop
//            for otherActor : = range actors{ // randomly iterates through everyone
//                if !otherActor.isSeller() {
//                    continue
//                }
//
//            // seller is asking for too high a price, move to the next one
//            if actor.expectedMarketValue < otherActor.expectedMarketValue {
//                continue
//            }
//
//                // passed all checks, transacting
//                actor.timeSinceLastTransaction, otherActor.timeSinceLastTransaction = 0, 0
//                actor.expectedMarketValue -= actor.beliefVolatility
//                otherActor.expectedMarketValue += actor.beliefVolatility
//                break
//            }
//        }
//
//    // if we haven't transacted in a while, update expected values
//    if actor.timeSinceLastTransaction > actor.maxTimeSinceLastTransaction{
//        actor.timeSinceLastTransaction = 0
//        if actor.isBuyer() {
//            // need to be willing to pay more
//            actor.expectedMarketValue += actor.beliefVolatility
//        }
//     else if actor.isSeller() {
//            // need to be willing to sell for lower
//            actor.expectedMarketValue -= actor.beliefVolatility
//        }
//    }
//}

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iomanip>
#include <locale>
#include <fstream>

using namespace std;


struct Agent {
    float money;
    float productivity;
    float reputation;

    float fraud_tendency;   // Crime chance 
    bool is_fraudulent;

    float last_money;       // learning memory
};

void updateReputation(Agent& a, float produced) {
    const float expected = 1.0;
    const float alpha = 0.05;
    const float decay = 0.99;

    float delta = (produced - expected) / expected;
    a.reputation += alpha * delta;
    a.reputation *= decay;

    a.reputation = (a.reputation, 0.0f, 1.0f);

}

void learn(Agent& a) {
    const float learning_rate = 0.02f;
    const float regret_rate = 0.05f;

    float payoff = a.money - a.last_money;

    if (a.is_fraudulent) {
        if (payoff > 0) {
            a.fraud_tendency += learning_rate * payoff;
        }
        else {
            a.fraud_tendency -= regret_rate * (-payoff);
        }
    }
    else {
        if (payoff < 0) {
            a.fraud_tendency += learning_rate * (-payoff);
        }
        else {
            a.fraud_tendency *= 0.999f; // slow decay toward honesty
        }
    }

    a.fraud_tendency = (a.fraud_tendency, 0.0f, 1.0f);
}

int main() {
    const uint64_t NUM_AGENTS = 100;
    const uint64_t TURNS = 200;
    float price = 1.00;

    ofstream rep_file("reputation_over_time.csv");
    rep_file << "Turn,AvgReputation,FraudsterReputation,HonestReputation,FraudsterCount\n";

    mt19937 rng(42);
    uniform_real_distribution<float> dist(0.5, 1.5);
    vector<Agent> agents;

    uniform_real_distribution<float> fraud_dist(0.0f, 1.0f);


    for (uint64_t i = 0; i < NUM_AGENTS; i++) {                 //initialisation
        float tendency = fraud_dist(rng);

        agents.push_back({
            10.0f,
            dist(rng),
            0.5f,
            tendency,
            false
            });
    }

    for (auto& j : agents) {
        j.last_money = j.money;
    }

    for (uint64_t turn = 0; turn < TURNS; turn++) {
        float total_supply = 0.0;
        float total_demand = NUM_AGENTS * 1.0;
        float total_rep = 0.0;

        for (auto& j : agents) {   //decides fraud
            j.is_fraudulent = false;

            if (fraud_dist(rng) < j.fraud_tendency) {
                j.is_fraudulent = true;
                total_rep += j.reputation ;  // Short term gain for realism
            }
            else {
                total_rep += j.reputation;
            }
        }

        for (auto& j : agents) {  //production
            float produced = j.productivity;
            total_supply += produced;
            total_rep += j.reputation;

            j.money += produced * price;
        }

        if (total_demand > total_supply)  //supply and demand price changes
            price *= 1.05;
        else
            price *= 0.95;


        for (auto& j : agents) {
            float produced = j.productivity;

            float effective_rep = j.is_fraudulent
                ? j.reputation * 1.5f
                : j.reputation;

            float share = (total_rep > 0)
                ? effective_rep / total_rep
                : 1.0f / NUM_AGENTS;

            float market_buy = share * total_demand;
            float sold = min(produced, market_buy);

            j.money += sold * price;

            updateReputation(j, produced);

            const float audit_rate = 0.05f; // strength of regulation

            if (j.is_fraudulent && fraud_dist(rng) < audit_rate) { // is caught
                j.reputation *= 0.2f; //reputation hit
                j.money *= 0.9f; // fine incurred
            }

            for (auto& a : agents) {
                learn(a);
            }

            float total_rep = 0.0f;
            float fraud_rep = 0.0f;
            float honest_rep = 0.0f;

            float avg_fraud_tendency = 0.0f;

            int fraud_count = 0;
            int honest_count = 0;

            for (auto& a : agents) {
                total_rep += a.reputation;
                avg_fraud_tendency += a.fraud_tendency;

                if (a.is_fraudulent) {
                    fraud_rep += a.reputation;
                    fraud_count++;
                }
                else {
                    honest_rep += a.reputation;
                    honest_count++;
                }
            }

            float avg_rep = total_rep / NUM_AGENTS;
            avg_fraud_tendency /= NUM_AGENTS;
            float avg_fraud_rep = fraud_count > 0 ? fraud_rep / fraud_count : 0.0f;
            float avg_honest_rep = honest_count > 0 ? honest_rep / honest_count : 0.0f;

            /*float avg_fraud_tendency = 0.0f;
            for (auto& a : agents)
                avg_fraud_tendency += a.fraud_tendency;

            avg_fraud_tendency /= NUM_AGENTS;*/

            rep_file << turn << ","
                << avg_rep << ","
                << avg_fraud_rep << ","
                << avg_honest_rep << ","
                << fraud_count << ","
                << avg_fraud_tendency << "\n";

        }

        for (auto& j : agents) {
            float desired = 1.0;            //desired to affordability check.
            float affordable = j.money / price;
            float bought = min(desired, affordable);
            j.money -= bought * price;
        }

        float total_money = 0;
        for (auto& j : agents)
            total_money += j.money;

        if (turn % 20 == 0) {
            //cout << setprecision(2) << price; - was trying to round to 2 decimal places and wasnt working will resolve later
            // weird issue where the debug console didnt like £ idk what the issue is will also resolve this later
            cout << fixed << setprecision(2);
            cout << "Turn " << turn
                << " | Price: GBP " << price
                << " | Avg Money: GBP " << total_money / NUM_AGENTS
                << endl;

            for (uint64_t i = 0; i < NUM_AGENTS; i++)
            {
                if (agents[i].reputation < 1)
                {
                    cout << "Agent Number : " << i
                        << " | Reputation :" << agents[i].reputation
                        << endl;
                }
            }
        }

        rep_file.close();

        
    }
}

