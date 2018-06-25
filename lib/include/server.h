#pragma once
#include <memory>
#include "agent.pb.h"
#include "common.h"
#include "sd.h"

namespace fetch {
  namespace oef {
    class AgentSession;

    class AgentDiscovery {
    private:
      mutable std::mutex _lock;
      std::unordered_map<std::string,std::shared_ptr<AgentSession>> _sessions;
    public:
      AgentDiscovery() = default;
      AgentDiscovery(const AgentDiscovery &) = delete;
      AgentDiscovery operator=(const AgentDiscovery &) = delete;
      bool exist(const std::string &id) const {
        return _sessions.find(id) != _sessions.end();
      }
      bool add(const std::string &id, std::shared_ptr<AgentSession> session) {
        std::lock_guard<std::mutex> lock(_lock);
        if(exist(id))
          return false;
        _sessions[id] = session;
        return true;
      }
      bool remove(const std::string &id) {
        std::lock_guard<std::mutex> lock(_lock);
        return _sessions.erase(id) == 1;
      }
      void clear() {
        std::lock_guard<std::mutex> lock(_lock);
        _sessions.clear();
      }
      std::shared_ptr<AgentSession> session(const std::string &id) const {
        std::lock_guard<std::mutex> lock(_lock);
        auto iter = _sessions.find(id);
        if(iter != _sessions.end()) {
          return iter->second;
        }
        return std::shared_ptr<AgentSession>(nullptr);
      }
      size_t size() const {
        std::lock_guard<std::mutex> lock(_lock);
        return _sessions.size();
      }
      std::vector<std::string> search(const QueryModel &query) const;
    };
    class Server {
    private:
      struct Context {
        tcp::socket _socket;
        uint32_t _len;
        std::vector<char> _buffer;
      Context(tcp::socket socket) : _socket{std::move(socket)} {}
      };
      
      // Careful: order matters.
      asio::io_context _io_context;
      std::unique_ptr<std::thread> _thread;
      tcp::acceptor _acceptor;
      AgentDiscovery _ad;
      ServiceDirectory _sd;
      
      void secretHandshake(const std::string &id, const std::shared_ptr<Context> &context);  
      void newSession(tcp::socket socket);
      void do_accept();
    public:
      explicit Server(uint32_t backlog = 256) :
      _acceptor(_io_context, tcp::endpoint(tcp::v4(), static_cast<int>(Ports::Agents))) {
        _acceptor.listen(backlog); // pending connections
      }

      Server(const Server &) = delete;
      Server operator=(const Server &) = delete;
      virtual ~Server();
      void run();
      void run_in_thread();
      size_t nbAgents() const { return _ad.size(); }
      void stop();
    };
  }
}
