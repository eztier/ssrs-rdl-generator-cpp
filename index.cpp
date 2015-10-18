#include <iostream>
#include "ssrs-rdl-generator.hpp"

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

int runWithBasicAuth(std::string host, std::string user, std::string password, std::string database, std::string script){
  int rc;
  //auto db = unique_ptr<tds::TDSClient>(new tds::TDSClient());
  auto db = tds::TDSClient();
  rc = db.connect(host, user, password);
  rc = db.useDatabase(database);
  db.sql(script);
  rc = db.execute();
  return 0;
}

int runSql(){
  std::string s = "select current_timestamp";
  std::string host = "localhost";
  std::string database = "JwsHRMS_US";
  std::string username = "joeschmoe";
  std::string password = "1234";

  ssrs::rdl::generator g(host, database, username, s);
  g.run();

  //runWithBasicAuth("localhost", "joeschmoe", "1234", "JwsHRMS_US", s);
  return 0;
}

int main(int argc, char *argv[]) {
  runSql();
}