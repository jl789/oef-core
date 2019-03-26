#pragma once
//------------------------------------------------------------------------------
//
//   Copyright 2018-2019 Fetch.AI Limited
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
//------------------------------------------------------------------------------

#include <memory>
#include <string>
#include "interface/agent_directory_t.hpp"
#include "interface/agent_session_t.hpp"
#include "interface/oefsearch_session_t.hpp"
#include "interface/communicator_t.hpp"

namespace fetch {
  namespace oef {      
    class core_server_t {
    private:
      //
      std::string core_id_;
      std::string lstn_ip_addr_;
      uint32_t lstn_port_;
      
      //
      //agent_directory_t agent_directory_;
      oefsearch_session_t* oefsearch_session_;
      
      virtual void do_accept(std::function<void(std::error_code,std::shared_ptr<communicator_t>)> continuation) = 0;
      virtual void process_agent_connection(const std::shared_ptr<communicator_t> communicator) = 0; 
    public:
      //
      virtual void run() = 0;
      virtual void run_in_thread() = 0;
      virtual void stop() = 0;
      virtual size_t nb_agents() const = 0;
      //
      virtual ~core_server_t() {};
    };
  }
}

