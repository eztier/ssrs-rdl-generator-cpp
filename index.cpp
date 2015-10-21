#include <iostream>
#include <regex>
#include <boost/filesystem.hpp>
#include <fstream>
#include "ssrs-rdl-generator.hpp"

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

auto setupLogging = []()->void {

  if (boost::filesystem::create_directory("./log")){
    boost::filesystem::path full_path(boost::filesystem::current_path());
    std::cout << "Successfully created directory"
      << full_path
      << "/log"
      << "\n";
  }

  size_t q_size = 1048576; //queue size must be power of 2
  spdlog::set_async_mode(q_size);

  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("log/ssrs-rdl-generator", "txt", 0, 0));
  auto combined_logger = std::make_shared<spdlog::logger>("logger", begin(sinks), end(sinks));
  combined_logger->set_pattern("[%Y-%d-%m %H:%M:%S:%e] [%l] [thread %t] %v");
  spdlog::register_logger(combined_logger);

};

namespace test {

  int runSql(){
    
    std::string s = "select oid, _webrunique_id \"uid\" from dbo.__fundingaward;";
    std::string host = "localhost";
    std::string database = "Grants";
    
    //windows authentication, just leave username and password blank
    ssrs::rdl::generator g(host, database, s);
    auto ctx = g.generateTemplateContext();
    auto r = g.compile(ctx);

    ofstream ofs("report2.rdl");
    ofs << *r;
    ofs.close();

    return 0;
  }
}

int main(int argc, char *argv[]) {

  setupLogging();

  test::runSql();
}