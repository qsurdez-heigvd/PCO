//     ____  __________     ___   ____ ___  _____ //
//    / __ \/ ____/ __ \   |__ \ / __ \__ \|__  / //
//   / /_/ / /   / / / /   __/ // / / /_/ / /_ <  //
//  / ____/ /___/ /_/ /   / __// /_/ / __/___/ /  //
// /_/    \____/\____/   /____/\____/____/____/   //
// Auteurs : Rachel Tranchida, Quentin Surdez
// Declaration of the ComputationManager class. We have added
// numerous variables in the protected field. These variables
// allow us to make the implementation of the methods.

// La déclaration de la classe ComputationManager se trouve en bas du fichier,
// ajoutez-y les attributs (et fonctions au besoin) pour votre implémentation.
//
// Ci-dessous se trouvent aussi les déclarations de divers classes et interfaces
// utilisées avec le buffer ComputationManager.

#ifndef COMPUTATIONMANAGER_H
#define COMPUTATIONMANAGER_H

// Ajoutez les includes dont vous avez besoin ici

#include <memory>
#include <list>
#include <map>
#include <optional>

#include "pcosynchro/pcohoaremonitor.h"

/**
 * @brief The ComputationType enum represents the abstract computation types that are available
 */
enum class ComputationType {
    A, B, C
};

/**
 * @brief The Computation class Represents a computation with a given type and data.
 */
class Computation {
public:
    /**
     * @brief Computation Constructs a computation of a given type
     * @param computationType
     */
    Computation(ComputationType computationType) : computationType(
            computationType) { data = std::make_shared<std::vector<double>>(); }

    /**
     * @brief computationType The given type
     */
    ComputationType computationType;
    /**
     * @brief data The data for the computation
     */
    std::shared_ptr<std::vector<double>> data;
};

/**
 * @brief The Request class is a request for a computation with and id and data
 */
class Request {
public:
    Request() : data(nullptr) {}

    Request(std::shared_ptr<std::vector<double>> data, int id) : data(std::move(data)), id(id) {}

    Request(const Computation &c, int id) : data(c.data), id(id) {}

    [[nodiscard]] int getId() const { return id; }

    /**
     * @brief data The data for the computation
     */
    std::shared_ptr<const std::vector<double>> data;

private:
    int id{0};
};

/**
 * @brief The Result class holds a result and an id
 */
class Result {
public:
    Result(int id, double result) : id(id), result(result) {}

    [[nodiscard]] const int getId() const { return id; }

    [[nodiscard]] double getResult() const { return result; }

private:
    int id;
    double result;
};

/**
 * @brief The ClientInterface class contains the methods of the buffer that are exposed to the client
 */
class ClientInterface {
public:
    /**
     * @brief requestComputation Request a computation c
     * @param c The computation to be done
     * @return The assigned id (should follow the order of the requests)
     */
    virtual int requestComputation(Computation c) = 0;

    /**
     * @brief abortComputation Allows the client to abort a computation
     * This should remove the Request/Result corresponding to the id in
     * the ComputationManager (buffer) if there is one and stop a compute
     * engine working on it if there was one.
     * @param id the id of the computation to be aborted
     */
    virtual void abortComputation(int id) = 0;

    /**
     * @brief getNextResult Method that provides the next result.
     * The order of the results must follow the order of the requests.
     * (order of ids of the results should follow the order of the request ids
     * which should follow the order of the requests).
     * @return The next Result of the requested computations
     */
    virtual Result getNextResult() = 0;
};

/**
 * @brief The ComputeEngineInterface class contains the methods of the buffer that are exposed to the compute engines
 */
class ComputeEngineInterface {
public:
    /**
     * @brief getWork is used to ask for work of a given type which will come as a Request
     * @param computationType the type of work that is wanted
     * @return a request to be fulfilled
     */
    virtual Request getWork(ComputationType computationType) = 0;

    /**
     * @brief continueWork Allows a compute engine to ask if it must continue working on a request
     * @param id the id of the request the compute engine is currently working on
     * @return true if the worker should continue working on the request with id id
     */
    virtual bool continueWork(int id) = 0;

    /**
     * @brief provideResult Allows a compute engine to prove a result to the buffer
     * @param result the result that has been computed
     */
    virtual void provideResult(Result result) = 0;
};

/**
 * @brief The ComputationManager class is the implementation of the shared buffer between client and compute engines.
 * It is to be implemented as a Hoare monitor.
 */
class ComputationManager : public ClientInterface, public ComputeEngineInterface, protected PcoHoareMonitor {
public:
    /**
     * @brief The StopException class is an exception that is thrown when a thread tries to wait
     * when the buffer is stopped or is awoken (signaled) after the buffer is stopped.
     */
    class StopException : public std::exception {
    };

    /**
     * @brief ComputationManager Allows to create a buffer with a maximum queue size
     * @param maxQueueSize the maximum queue size allowed to store pending requests
     */
    ComputationManager(int maxQueueSize = 10);

    // Client Interface
    // Documentation above
    int requestComputation(Computation c) override;

    void abortComputation(int id) override;

    Result getNextResult() override;

    // Compute Engine Interface
    // Documentation above
    Request getWork(ComputationType computationType) override;

    bool continueWork(int id) override;

    void provideResult(Result result) override;

    bool checkIsBufferFull();


    // Control Interface
    /**
     * @brief stop Is used when the buffer is stopped, will release and interrupt waiting threads
     */
    void stop();

protected:

    // Ajoutez vos attributs et déclarations de méthodes ici
    // P.ex. variables conditions et structure de données pour le buffer

    // Queues
    const size_t MAX_TOLERATED_QUEUE_SIZE;
    Condition resultNotAvailable;
    std::map<ComputationType, Condition> isBufferFull;
    std::map<ComputationType, Condition> isBufferEmpty;
    bool stopped = false;

private:
    /**
     * @brief throwStopException Throws a StopException (will be handled by the caller)
     */
    inline void throwStopException() { throw StopException(); }

    int nextId = 0;
    std::map<ComputationType, std::list<Request>> requestBuffer;
    std::list<std::pair<id_t, std::optional<Result>>> results;
};

#endif // COMPUTATIONMANAGER_H
