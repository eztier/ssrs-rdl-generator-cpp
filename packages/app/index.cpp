#include <iostream>
#include "ssrs-rdl-generator.hpp"

using namespace std;

int runSql(){
    
  std::string s = "select fa.oid, _webrunique_id \"uid\", r.name, r.dateModified from dbo.__fundingaward fa inner join dbo._resource r on r.oid = fa.oid;";
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

auto main(int argc, char *argv[]) -> int {
  runSql();
}