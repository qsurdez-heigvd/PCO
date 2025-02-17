//     ____  __________     ___   ____ ___  _____ //
//    / __ \/ ____/ __ \   |__ \ / __ \__ \|__  / //
//   / /_/ / /   / / / /   __/ // / / /_/ / /_ <  //
//  / ____/ /___/ /_/ /   / __// /_/ / __/___/ /  //
// /_/    \____/\____/   /____/\____/____/____/   //
// Auteurs : Quentin Surdez, Rachel Tranchida
// Implementation of all methods from ComputationManager.h 


// A vous de remplir les méthodes, vous pouvez ajouter des attributs ou méthodes pour vous aider
// déclarez les dans ComputationManager.h et définissez les méthodes ici.
// Certaines fonctions ci-dessous ont déjà un peu de code, il est à remplacer, il est là temporairement
// afin de faire attendre les threads appelants et aussi afin que le code compile.

#include <algorithm>
#include <pcosynchro/pcohoaremonitor.h>
#include "computationmanager.h"

/**
 * @brief A computation manager allows to handle many computations and give back their result in order
*/
ComputationManager::ComputationManager(int maxQueueSize) : MAX_TOLERATED_QUEUE_SIZE(maxQueueSize) {

}

/**
 * @brief Function used to make a request for a computation of a certain type
 * @param c the type of the computation to make
 * 
 * @return id of the newly created request
*/
int ComputationManager::requestComputation(Computation c) {
    monitorIn();
    int id = nextId++;
    if (requestBuffer[c.computationType].size() >= MAX_TOLERATED_QUEUE_SIZE) {

        if (stopped) {
            monitorOut();
            throwStopException();
        }
        wait(isBufferFull[c.computationType]);

        if (stopped) {
            monitorOut();
            signal(isBufferFull[c.computationType]);
            throwStopException();
        }
    }


    requestBuffer[c.computationType].emplace_back(c, id);
    results.emplace_back(id, std::optional<Result>{});
    signal(isBufferEmpty[c.computationType]);

    monitorOut();
    return id;
}

/**
 * @brief Function to abort a request
 * @param id unique id of the request of interest
*/
void ComputationManager::abortComputation(int id) {
    monitorIn();
    ComputationType type;
    // we look for the request in the requestBuffer and erase it
    for (auto &requestType: requestBuffer) {
        // we look for request of parameter id
        auto index = std::find_if(requestType.second.begin(), requestType.second.end(),
                                  [id](const auto &request) { return request.getId() == id; });
        if (index != requestType.second.end()) {
            type = requestType.first;
            requestType.second.erase(index);
            signal(isBufferFull[type]);
            break;
        }
    }

    // we look for the request in results
    auto resultIndex = std::find_if(results.begin(), results.end(), [id](auto &result) { return result.first == id; });
    if (resultIndex != results.end()) {
        results.erase(resultIndex);

    }

    if (results.front().second.has_value()) {
        signal(resultNotAvailable);
    }


    monitorOut();
}

/**
 * @brief Function that returns the result of the next computation
 * 
 * @return result of the next in line computation
*/
Result ComputationManager::getNextResult() {

    monitorIn();
    while (!stopped) {
        if (!results.empty() && results.front().second.has_value()) {
            const auto &[id, result] = results.front();

            results.pop_front();
            monitorOut();
            return result.value();

        }
        wait(resultNotAvailable);
    }
    signal(resultNotAvailable);
    monitorOut();
    throwStopException();

}

/**
 * @brief Function to make the computation work
 * @param computationType the type of interest
 * 
 * @return the request linked to the computation
*/
Request ComputationManager::getWork(ComputationType computationType) {

    monitorIn();

    if (requestBuffer[computationType].empty()) {
        if (stopped) {
            monitorOut();
            throwStopException();
        }
        wait(isBufferEmpty[computationType]);
        if (stopped) {
            monitorOut();
            // we signal the next one
            signal(isBufferEmpty[computationType]);
            throwStopException();
        }
    }


    Request request = requestBuffer[computationType].front();
    requestBuffer[computationType].pop_front();
    signal(isBufferFull[computationType]);
    monitorOut();

    return request;
}

/**
 * @brief Function to check if the work can be continued or not
 * @param id unique id of the request of interest
 * 
 * @return bool that indicates if the work must continue or not
*/
bool ComputationManager::continueWork(int id) {

    monitorIn();
    if (stopped) {
        monitorOut();
        return false;

    }
    // if the id is in result buffer we can continue work
    if (std::find_if(results.begin(), results.end(),
                     [id](std::pair<int, std::optional<Result>> const &r) { return r.first == id; }) != results.end()) {
        monitorOut();
        return true;
    }
    monitorOut();
    return false;
}

/**
 * @brief Function to give the next result
 * @param result the result of interest
*/
void ComputationManager::provideResult(Result result) {

    monitorIn();
    id_t id = result.getId();
    auto pos = std::find_if(results.begin(), results.end(),
                            [id](std::pair<int, std::optional<Result>> const &b) {
                                return b.first == id;
                            });
    if (pos == results.end()) {
        monitorOut();
        return;
    }

    pos->second = result;
    // if the result we just had was the first result awaited in queue we signal
    if (pos->first == results.front().first) {
        signal(resultNotAvailable);
    }
    monitorOut();
}

/**
 * @brief Function to stop all threads
*/
void ComputationManager::stop() {

    monitorIn();

    stopped = true;
    // we free all threads waiting the buffer not to be full
    std::for_each(
            isBufferFull.begin(),
            isBufferFull.end(),
            [this](auto &pair) { signal(pair.second); }
    );
    // we free all threads waiting the buffer not to be empty
    std::for_each(
            isBufferEmpty.begin(),
            isBufferEmpty.end(),
            [this](auto &pair) { signal(pair.second); }
    );
    // we free all threads waiting for result to be available
    signal(resultNotAvailable);

    monitorOut();

}
