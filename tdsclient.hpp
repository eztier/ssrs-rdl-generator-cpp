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

  static const std::unordered_map<int, int> binaryTypes = { { SYBIMAGE, 34 }, { SYBTEXT, 35 }, { SYBNTEXT, 99 }, { SYBBINARY, 45 }, { SYBVARBINARY, 37 } };
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
    char nullBuffer[1] = "";
  };

}


/*
ref: http://lists.ibiblio.org/pipermail/freetds/2004q3/016451.html

*/
int err_handler(DBPROCESS* dbproc, int severity, int dberr, int oserr, char* dberrstr, char* oserrstr) {
  if ((dbproc == NULL) || (DBDEAD(dbproc))) {
    string err = "dbproc is NULL error: ";
    err.append(dberrstr);
    
    return(INT_CANCEL);
  }
  else {
    cerr << "DB-Library error: {} {}" << dberr << " " << dberrstr << endl;

    if (oserr != DBNOERR) {
      cerr << "Operating-system error: {}" << oserrstr << endl;
    }
    // DO NOT CALL dbclose(dbproc)! or dbexit(); tds needs to clean itself up!
    return(INT_CANCEL);
  }
}

int msg_handler(DBPROCESS* dbproc, DBINT msgno, int msgstate, int severity, char* msgtext, char* srvname, char* procname, int line) {
  /*
  ** If it's a database change message, we'll ignore it.
  ** Also ignore language change message.
  */
  if (msgno == 5701 || msgno == 5703)
    return(0);

  // spdlog::get(loggerName)->warn("msgno: {} severity: {} msgstate: {}", msgno, severity, msgstate);
  cout << "msgno: {} severity: {} msgstate: {}" << msgno << severity << msgstate << endl;

  if (strlen(srvname) > 0)
    // spdlog::get(loggerName)->warn("Server: {}", srvname);

  if (strlen(procname) > 0)
    // spdlog::get(loggerName)->warn("Procedure: {}", procname);

  if (line > 0)
    // spdlog::get(loggerName)->warn("line: {}", line);

  // spdlog::get(loggerName)->warn("msgtext: {}", msgtext);

  return(0);
}

tds::TDSClient::~TDSClient() {

}

int tds::TDSClient::connect(string& _host, string& _user, string& _pass){
  host = _host;
  user = _user;
  pass = _pass;
  return connect();
};

//windows nt
int tds::TDSClient::connect(string& _host){
  host = _host;
  return connect();
};

/*
do not call dbinit multiple times!
http://codeverge.com/sybase.ase.unix/memory-leak-in-libsybdb.so-2/966326
*/
int tds::TDSClient::init() {
  
  if (dbinit() == FAIL) {
    cerr << "dbinit() failed" << endl;
    return 1;
  }

  this->rows = make_unique<TDSRows>();

  return 0;
};

int tds::TDSClient::connect() {

  int rc = this->init();

  if (rc){
    return rc;
  }

  //handle server/network errors
  dberrhandle(err_handler);
  dbmsghandle(msg_handler);

  // Get a LOGINREC for logging in
  if ((login = dblogin()) == NULL) {
    cerr << "connect() unable to allocate login structure" << endl;
    return 1;
  }

  if (pass.size() > 0){
    DBSETLUSER(login, user.c_str());
  }
  else {
    DBSETLUSER(login, "");
  }
  if (pass.size() > 0){
    //non windows nt
    DBSETLPWD(login, pass.c_str());
  }
  else {
    DBSETLPWD(login, "");
  }
  DBSETLAPP(login, "ssrs-rdl-generator");

  //set timeout for login
  dbsetlogintime(5);

  this->dbproc = dbopen(login, host.c_str());

  // Frees the login record, can be called immediately after dbopen.
  dbloginfree(login);

  // Connect to server
  if (this->dbproc == NULL) {
    cerr << "connect() unable to connect to {}" << host;
    return 1;
  }
  return 0;

};

int tds::TDSClient::useDatabase(string& db){
  if ((erc = dbuse(dbproc, db.c_str())) == FAIL) {
    cerr << "useDatabase() unable to use database {}" << db << endl;

    this->close();
    return 1;
  }
  return 0;
};

void tds::TDSClient::sql(string& _script){
  script = _script;
  dbcmd(dbproc, script.c_str());
};

int tds::TDSClient::getMetadata() {

  ncols = dbnumcols(dbproc);
  
  if ((columns = (COL*)calloc(ncols, sizeof(struct COL))) == NULL) {
    perror(NULL);
    return 1;
  }

  /*
  * Read metadata and bind.
  */

  for (pcol = columns; pcol - columns < ncols; pcol++) {

    int c = pcol - columns + 1;

    pcol->name = dbcolname(dbproc, c);
    pcol->type = dbcoltype(dbproc, c);
    pcol->size = dbcollen(dbproc, c);

    TDSMeta m(pcol->name, pcol->type, pcol->size);
    auto meta = make_shared<TDSCell<TDSMeta>>(m);
    rows->fieldMetas->push_back(meta);

    //default to string with max size of 255
    if (SYBCHAR != pcol->type) {
      pcol->size = dbprcollen(dbproc, c);
      if (pcol->size > 255)
        pcol->size = 255;
    }

    auto col = make_shared<TDSCell<string>>(pcol->name);
    rows->fieldNames->push_back(col);

    if ((pcol->buffer = (char*)calloc(1, pcol->size + 1)) == NULL){
      perror(NULL);
      return 1;
    }

    erc = dbbind(dbproc, c, NTBSTRINGBIND, pcol->size + 1, (BYTE*)pcol->buffer);
    
    if (erc == FAIL) {
      cerr << "dbnullbind {} failed " << c << endl; 
      return 1;
    }

    erc = dbnullbind(dbproc, c, &pcol->status);

    if (erc == FAIL) {
      cerr << "dbnullbind {} failed " << c << endl;
      return 1;
    }
  }

  return 0;
};

int tds::TDSClient::fetchData() {

  while ((row_code = dbnextrow(dbproc)) != NO_MORE_ROWS){

    auto row = make_shared<RowOfString>();

    switch (row_code) {
    case REG_ROW:
      for (pcol = columns; pcol - columns < ncols; pcol++) {
        char *buffer = pcol->status == -1 ? nullBuffer : pcol->buffer;
        
        auto v = make_shared<TDSCell<string>>(buffer);
        row->push_back(v);
      }

      rows->fieldValues->push_back(row);
      break;

    case BUF_FULL:
      assert(row_code != BUF_FULL);
      break;

    case FAIL:
      cerr << "dbresults failed" << endl;
      return 1;
      break;

    default:
      printf("Data for computeid %d ignored\n", row_code);
    }

  }

  /* free metadata and data buffers */
  for (pcol = columns; pcol - columns < ncols; pcol++) {
    free(pcol->buffer);
  }
  free(columns);

  return 0;
};

int tds::TDSClient::execute() {

  auto status = dbsqlexec(dbproc);

  if (status == FAIL) {
    cerr << "execute() dbsqlexec failed" << endl;
    return 1;
  }

  while ((erc = dbresults(dbproc)) != NO_MORE_RESULTS) {

    if (erc == FAIL) {
      cerr << "execute() no results" << endl;
      return 1;
    }

    /*
    * Read metadata and bind.
    */
    getMetadata();

    /*
    fetch data
    */
    fetchData();
  }

  this->close();
  
  return 0;

};

void tds::TDSClient::close() {
  if (this->dbproc != NULL){
    dbclose(dbproc);
    this->dbproc = NULL;
  }
  dbexit();
}

#endif
