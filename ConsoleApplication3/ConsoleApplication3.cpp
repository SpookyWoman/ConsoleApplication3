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

using namespace std;


struct Agent {
    float money;
    float productivity;
    float reputation;
};

void updateReputation(Agent& a, float produced) {
    const float expected = 1.0;
    const float alpha = 0.05;
    const float decay = 0.995;

    float delta = (produced - expected) / expected;
    a.reputation += alpha * delta;
    a.reputation *= decay;

    a.reputation = (a.reputation, 0.0, 1.0);
}

int main() {
    const int NUM_AGENTS = 100;
    const int TURNS = 200;
    float price = 1.00;

    mt19937 rng(42);
    uniform_real_distribution<float> prod_dist(0.5, 1.5);
    vector<Agent> agents;

    for (int i = 0; i < NUM_AGENTS; i++) {
        agents.push_back({ 10.0, prod_dist(rng) });
    }

    for (int turn = 0; turn < TURNS; turn++) {
        float total_supply = 0.0;
        float total_demand = NUM_AGENTS * 1.0;
        float total_rep = 0.0;

        for (auto& a : agents) {
            float produced = a.productivity;
            total_supply += produced;
            total_rep += a.reputation;

            a.money += produced * price;
        }

        if (total_demand > total_supply)
            price *= 1.05;
        else
            price *= 0.95;

        for (auto& a : agents) {
            float produced = a.productivity;

            float share = (total_rep > 0)
                ? (a.reputation / total_rep)
                : (1.0 / NUM_AGENTS);

            float market_buy = share * total_demand;
            float sold = min(produced, market_buy);

            a.money += sold * price;

            updateReputation(a, produced);
        }

        for (auto& a : agents) {
            float desired = 1.0;
            float affordable = a.money / price;
            float bought = min(desired, affordable);
            a.money -= bought * price;
        }

        float total_money = 0;
        for (auto& a : agents)
            total_money += a.money;

        if (turn % 20 == 0) {
            //cout << setprecision(2) << price; - was trying to round to 2 decimal places and wasnt working will resolve later
            // weird issue where the debug console didnt like £ idk what the issue is will also resolve this later
            cout << "Turn " << turn
                << " | Price: " << price
                << " | Avg Money: " << total_money / NUM_AGENTS
                << endl;
        }
    }
}

