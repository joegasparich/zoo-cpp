#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <vector>
#include <map>
#include <functional>

using namespace std;
using namespace boost::uuids;

class Mediator {
public:
    static Mediator& getInstance() {
        static Mediator theInstance;
        instance = &theInstance;
        return *instance;
    }

    uuid on(const string& event, function<void(string)>);
    void fire(const string& event, const string& data);
    void unsubscribe(const string& event, uuid handle);

private:
    static Mediator *instance;
    Mediator();
    ~Mediator() noexcept;
    Mediator(const Mediator& rs) { instance = rs.instance; }
    Mediator& operator=(const Mediator& rs) {
        if (this != &rs) {
            instance = rs.instance;
        }
        return *this;
    }

    map<string, map<uuid, function<void(string)>>> listeners;
};
