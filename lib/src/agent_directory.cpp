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

#include "agent_directory.hpp"

namespace fetch {
namespace oef {

fetch::oef::Logger AgentDirectory_::logger = fetch::oef::Logger("agent-directory");

const std::vector<std::string> AgentDirectory_::search(const QueryModel &query) const {
  std::lock_guard<std::mutex> lock(lock_);
  std::vector<std::string> res;
  for(const auto &s : sessions_) {
    if(s.second->match(query)) {
      res.emplace_back(s.first);
    }
  }
  return res;
}

} // oef
} // fethc