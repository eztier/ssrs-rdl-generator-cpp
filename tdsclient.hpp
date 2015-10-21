#ifndef TDSCLIENT_H
#define TDSCLIENT_H

#include <iostream>
#include <vector>
//#include <memory>
#include <sybfront.h>	/* sybfront.h always comes first */
#include <sybdb.h>	/* sybdb.h is the only other file you need */
#include "spdlog/spdlog.h"

using namespace std;

namespace tds{

  static const std::unordered_map<int, int> binaryTypes = { { SYBIMAGE, 34 }, { SYBTEXT, 35 }, { SYBBINARY, 45 }, { SYBVARBINARY, 37 } };
  static const std::unordered_map<int, int> dateTypes = { { SYBDATETIME, 61 }, { SYBDATETIME4, 58 }, { SYBDATETIMN, 111 } };

  class TDSMeta {
  public:
    std::string name;
    int type, size;
    TDSMeta(std::string _name, int _type, int _size) : name(_name), type(_type), size(_size) {}
    ~TDSMeta(){}
  };

  template<typename T>
  class TDSCell {
  public:
    T value;
    TDSCell<T>(T _val) : value(_val){}
    ~TDSCell<T>(){}
  };

  typedef vector<shared_ptr<TDSCell<TDSMeta>>> RowOfMeta;
  typedef vector<shared_ptr<TDSCell<string>>> RowOfString;
  typedef vector<shared_ptr<vector<shared_ptr<TDSCell<string>>>>> TableOfRowsOfString;

  class TDSRows {
  public:
    shared_ptr<RowOfMeta> fieldMetas;
    shared_ptr<RowOfString> fieldNames;
    shared_ptr<TableOfRowsOfString> fieldValues;
    TDSRows() {
      /*
      initialize shared ptrs
      */
      fieldNames = make_shared<RowOfString>();
      fieldValues = make_shared<TableOfRowsOfString>();
      fieldMetas = make_shared<RowOfMeta>();
    }
    ~TDSRows(){
    }
  };

  class TDSClient{
  public:

    struct COL
    {
      char *name;
      char *buffer;
      int type, size, status;
    } *columns, *pcol;

    int init();
    int connect();
    int connect(string& _host, string& _user, string& _pass);
    int connect(string& _host);
    int useDatabase(string& _db);
    void sql(string& _script);
    int execute();
    int getMetadata();
    int fetchData();
    void close();
    TDSClient(){};
    unique_ptr<TDSRows> rows;
    TDSClient(string& _host, string& _user, string& _pass) : host(_host), user(_user), pass(_pass) {}
    TDSClient(string& _host) : host(_host) {}
    ~TDSClient();
    DBPROCESS *dbproc = NULL;
  private:
    string host;
    string user;
    string pass;
    string script;
    int ncols;
    int row_code;
    LOGINREC *login = NULL;
    RETCODE erc;
  };

}
#endif