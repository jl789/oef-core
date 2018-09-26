#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "server.h"
#include <iostream>
#include <chrono>
#include <future>
#include "sd.h"
#include "multiclient.h"

using fetch::oef::Server;
using fetch::oef::MultiClient;

class SimpleAgent : public fetch::oef::AgentInterface, public fetch::oef::OEFCoreNetworkProxy {
 private:
  fetch::oef::OEFCoreProxy _oefCore;
  
 public:
  std::vector<std::string> _results;
  SimpleAgent(const std::string &agentId, asio::io_context &io_context, const std::string &host)
    : fetch::oef::OEFCoreNetworkProxy{agentId, io_context, host}, _oefCore{*this, *this} {
  }
  void onError(fetch::oef::pb::Server_AgentMessage_Error_Operation operation, const std::string &conversationId, uint32_t msgId) override {}
  void onSearchResult(const std::vector<std::string> &results) override {
    _results = results;
  }
  void onMessage(const std::string &from, const std::string &conversationId, const std::string &content) override {}
  void onCFP(const std::string &from, const std::string &conversationId, uint32_t msgId, uint32_t target, const stde::optional<QueryModel> &constraints) override {}
  void onPropose(const std::string &from, const std::string &conversationId, uint32_t msgId, uint32_t target, const std::vector<Instance> &proposals) override {}
  void onAccept(const std::string &from, const std::string &conversationId, uint32_t msgId, uint32_t target, const std::vector<Instance> &proposals) override {}
  void onClose(const std::string &from, const std::string &conversationId, uint32_t msgId, uint32_t target) override {}
 };

TEST_CASE("testing register", "[ServiceDiscovery]") {
  fetch::oef::Server as;
  std::cerr << "Server created\n";
  as.run();
  std::cerr << "Server started\n";
  std::cerr << "Waiting\n";
  std::this_thread::sleep_for(std::chrono::seconds{1});
  std::cerr << "NbAgents " << as.nbAgents() << "\n";
  REQUIRE(as.nbAgents() == 0);
  {
    IoContextPool pool(2);
    pool.run();
    SimpleAgent c1("Agent1", pool.getIoContext(), "127.0.0.1");
    SimpleAgent c2("Agent2", pool.getIoContext(), "127.0.0.1");
    SimpleAgent c3("Agent3", pool.getIoContext(), "127.0.0.1");
    REQUIRE(as.nbAgents() == 3);
    Attribute manufacturer{"manufacturer", Type::String, true};
    Attribute colour{"colour", Type::String, false};
    Attribute luxury{"luxury", Type::Bool, true};
    DataModel car{"car", {manufacturer, colour, luxury}, "Car sale."};
    Instance ferrari{car, {{"manufacturer", "Ferrari"}, {"colour", "Aubergine"}, {"luxury", "true"}}};
    c1.registerService(ferrari);
    std::this_thread::sleep_for(std::chrono::seconds{1});
    c1.unregisterService(ferrari);
    std::this_thread::sleep_for(std::chrono::seconds{1});
    c1.registerService(ferrari);
    Instance lamborghini{car, {{"manufacturer", "Lamborghini"}, {"luxury", "true"}}};
    c2.registerService(lamborghini);
    std::this_thread::sleep_for(std::chrono::seconds{1});
    ConstraintType eqTrue{Relation{Relation::Op::Eq, true}};
    Constraint luxury_c{luxury, eqTrue};
    QueryModel q1{{luxury_c}, car};
    c3.searchServices(q1);
    std::this_thread::sleep_for(std::chrono::seconds{1});
    auto agents = c3._results;
    std::sort(agents.begin(), agents.end());
    REQUIRE(agents.size() == 2);
    REQUIRE(agents == std::vector<std::string>({"Agent1", "Agent2"}));
    c1.stop();
    c2.stop();
    c3.stop();
    pool.stop();
    std::cerr << "Agent3 received\n";
    for(auto &s : agents) {
      std::cerr << s << std::endl;
    }
  }
  //  std::this_thread::sleep_for(std::chrono::seconds{1});
  as.stop();
  std::cerr << "Server stopped\n";
}

TEST_CASE("description", "[ServiceDiscovery]") {
  fetch::oef::Server as;
  std::cerr << "Server created\n";
  as.run();
  std::cerr << "Server started\n";
  std::cerr << "Waiting\n";
  std::this_thread::sleep_for(std::chrono::seconds{1});
  std::cerr << "NbAgents " << as.nbAgents() << "\n";
  REQUIRE(as.nbAgents() == 0);
  {
    IoContextPool pool(2);
    pool.run();
    SimpleAgent c1("Agent1", pool.getIoContext(), "127.0.0.1");
    SimpleAgent c2("Agent2", pool.getIoContext(), "127.0.0.1");
    SimpleAgent c3("Agent3", pool.getIoContext(), "127.0.0.1");
    REQUIRE(as.nbAgents() == 3);

    Attribute manufacturer{"manufacturer", Type::String, true};
    Attribute model{"model", Type::String, true};
    Attribute wireless{"wireless", Type::Bool, true};
    DataModel station{"weather_station", {manufacturer, model, wireless}, "Weather station"};
    Instance youshiko{station, {{"manufacturer", "Youshiko"}, {"model", "YC9315"}, {"wireless", "true"}}};
    Instance opes{station, {{"manufacturer", "Opes"}, {"model", "17500"}, {"wireless", "true"}}};
    c1.registerDescription(youshiko);
    c2.registerDescription(opes);
    std::this_thread::sleep_for(std::chrono::seconds{1});
    ConstraintType eqTrue{Relation{Relation::Op::Eq, true}};
    Constraint wireless_c{wireless, eqTrue};
    QueryModel q1{{wireless_c}, station};
    c3.searchAgents(q1);
    std::this_thread::sleep_for(std::chrono::seconds{1});
    auto agents = c3._results;
    std::sort(agents.begin(), agents.end());
    REQUIRE(agents.size() == 2);
    REQUIRE(agents == std::vector<std::string>({"Agent1", "Agent2"}));
    ConstraintType eqYoushiko{Relation{Relation::Op::Eq, std::string{"Youshiko"}}};
    Constraint manufacturer_c{manufacturer, eqYoushiko};
    QueryModel q2{{manufacturer_c}};
    c3.searchAgents(q2);
    std::this_thread::sleep_for(std::chrono::seconds{1});
    auto agents2 = c3._results;
    REQUIRE(agents2.size() == 1);
    REQUIRE(agents2 == std::vector<std::string>({"Agent1"}));
    
    c1.stop();
    c2.stop();
    c3.stop();
    pool.stop();
  }
  as.stop();
  std::cerr << "Server stopped\n";
}

TEST_CASE( "testing Server", "[Server]" ) {
  fetch::oef::Server as;
  std::cerr << "Server created\n";
  as.run();
  std::cerr << "Server started\n";
  REQUIRE(as.nbAgents() == 0);
  
  SECTION("1 agent") {
    IoContextPool pool(2);
    pool.run();
    SimpleAgent c1("Agent1", pool.getIoContext(), "127.0.0.1");
    REQUIRE(as.nbAgents() == 1);
    c1.stop();
    pool.stop();
  }
  std::this_thread::sleep_for(std::chrono::seconds{1});
  REQUIRE(as.nbAgents() == 0);
  //too fast ?
  SECTION("1000 agents") {
    // need to increase max nb file open
    // > ulimit -n 8000
    // ulimit -n 1048576
    
    IoContextPool pool(2);
    pool.run();
    std::vector<std::unique_ptr<SimpleAgent>> clients;
    std::vector<std::future<std::unique_ptr<SimpleAgent>>> futures;
    size_t nbClients = 1000;
    try {
      for(size_t i = 1; i <= nbClients; ++i) {
        std::string name = "Agent_";
        name += std::to_string(i);
        futures.push_back(std::async(std::launch::async, [&pool](const std::string &n){
                                                           return std::make_unique<SimpleAgent>(n, pool.getIoContext(), "127.0.0.1");
                                                         }, name));
      }
      std::cerr << "Futures created\n";
      for(auto &fut : futures) {
        clients.emplace_back(fut.get());
      }
      std::cerr << "Futures got\n";
    } catch(std::exception &e) {
      std::cerr << "BUG " << e.what() << "\n";
    }
    pool.stop();
    REQUIRE(as.nbAgents() == nbClients);
  }
  std::this_thread::sleep_for(std::chrono::seconds{1});
  REQUIRE(as.nbAgents() == 0);
  
  as.stop();
  std::cerr << "Server stopped\n";
}

class SimpleMultiClient : public MultiClient<bool,SimpleMultiClient> {
public:
  SimpleMultiClient(asio::io_context &io_context, const std::string &id, const std::string &host) :
    fetch::oef::MultiClient<bool,SimpleMultiClient>{io_context, id, host} {}
  void onMsg(const fetch::oef::pb::Server_AgentMessage &msg, fetch::oef::Conversation<bool> &Conversation) {
  }
};

TEST_CASE( "testing multiclient", "[Client]" ) {
  fetch::oef::Server as;
  std::cerr << "Server created\n";
  as.run();
  std::cerr << "Server started\n";
  REQUIRE(as.nbAgents() == 0);
  SECTION("1 agent") {
    IoContextPool pool(1);
    pool.run();
    SimpleMultiClient c1(pool.getIoContext(), "Agent1", "127.0.0.1");
    std::cerr << "Debug1 before stop" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds{1});
    REQUIRE(as.nbAgents() == 1);
    pool.stop();
    //    c1.stop();
    std::cerr << "Debug1 after stop" << std::endl;
  }
  std::this_thread::sleep_for(std::chrono::seconds{1});
  REQUIRE(as.nbAgents() == 0);
  //too fast ?
  
  SECTION("1000 agents") {
    // need to increase max nb file open
    // > ulimit -n 8000
    // ulimit -n 1048576
    IoContextPool pool(10);
    pool.run();

    std::vector<std::unique_ptr<SimpleMultiClient>> clients;
    std::vector<std::future<std::unique_ptr<SimpleMultiClient>>> futures;
    size_t nbClients = 10;
    try {
      for(size_t i = 1; i <= nbClients; ++i) {
        std::string name = "Agent_";
        name += std::to_string(i);
        futures.push_back(std::async(std::launch::async,
                                     [&pool](const std::string &n){
                                       return std::make_unique<SimpleMultiClient>(pool.getIoContext(), n, "127.0.0.1");
                                     }, name));
      }
      std::cerr << "Futures created\n";
      for(auto &fut : futures) {
        clients.emplace_back(fut.get());
      }
      std::cerr << "Futures got\n";
    } catch(std::exception &e) {
      std::cerr << "BUG " << e.what() << "\n";
    }
    std::this_thread::sleep_for(std::chrono::seconds{1});
    REQUIRE(as.nbAgents() == nbClients);
    pool.stop();
  }
  std::this_thread::sleep_for(std::chrono::seconds{1});
  REQUIRE(as.nbAgents() == 0);
  
  as.stop();
  std::cerr << "Server stopped\n";
}
