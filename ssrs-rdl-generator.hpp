#ifndef ALLENTOWN_HPP
#define ALLENTOWN_HPP

#include <iostream>
#include "tdsclient.hpp"
#include "plustache/template.hpp"
#include "plustache/plustache_types.hpp"
#include "plustache/context.hpp"

namespace ssrs {
  namespace rdl {

    const std::string nm("http://schemas.microsoft.com/sqlserver/reporting/2009/01/reportdefinition");

    namespace XmlElement {
      class Root;
      class DataSources;  
      class DataSets; 
      class ReportSections; 
      class TablixColumns; 
      class TablixRows; 
      class TablixColumnHierarchy; 
      class TablixRowHierarchy; 
    };

    template<typename xmlelement>
    class base {
    public: 
      base(){}
      ~base(){}
      std::string tpl;

      template<class T>
      int compile(const string tplFile, const T& o){

        //read entire template file 
        std::ifstream file(tplFile.c_str());
        std::string tpl((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        if (tpl.size() == 0){
          return 1;
        }

        //compile tempplate with context to create a soap message
        Plustache::template_t t;
        xml = t.render(tpl, o);

        return 0;
      }
    };

    template<typename xmlelement>
    class generator_impl : public base<xmlelement>{};

    /*
    DataSources
    */
    template<>
    class generator_impl<XmlElement::Root> : public base<XmlElement::Root>{
    public:
      generator_impl(){ tpl = "tpl/root"; }

    };

    /*
      DataSources
    */
    template<>
    class generator_impl<XmlElement::DataSources> : public base<XmlElement::DataSources>{
    public:
      generator_impl(){ tpl = "tpl/datasources"; }

    };

    /*
      DataSets
    */
    template<>
    class generator_impl<XmlElement::DataSets> : public base<XmlElement::DataSets>{
    public:
      generator_impl(){ tpl = "tpl/datasets"; }

    };

    /*
      ReportSections
    */
    template<>
    class generator_impl<XmlElement::ReportSections> : public base<XmlElement::ReportSections>{
    public:
      generator_impl(){ tpl = "tpl/report-sections"; }

    };

    /*
      TablixColumns
    */
    template<>
    class generator_impl<XmlElement::TablixColumns> : public base<XmlElement::TablixColumns>{
    public:
      generator_impl(){ tpl = "tpl/tablix-columns"; }

    };

    /*
      TablixRows
    */
    template<>
    class generator_impl<XmlElement::TablixRows> : public base<XmlElement::TablixRows>{
    public:
      generator_impl(){ tpl = "tpl/tablix-rows"; }

    };

    /*
      TablixColumnHierarchy
    */
    template<>
    class generator_impl<XmlElement::TablixColumnHierarchy> : public base<XmlElement::TablixColumnHierarchy>{
    public:
      generator_impl(){ tpl = "tpl/tablix-column-hierarchy"; }

    };

    /*
      TablixRowHierarchy
    */
    template<>
    class generator_impl<XmlElement::TablixRowHierarchy> : public base<XmlElement::TablixRowHierarchy>{
    public:
      generator_impl(){ tpl = "tpl/tablix-row-hierarchy"; }

    };

    class generator {
       
    public:
      generator(std::string _host, std::string _database, std::string _user, std::string _password, std::string _script) : script(_script){}
      generator(std::string _host, std::string _database, std::string _user, std::string _script) : script(_script){}
      int run(){

        auto db = unique_ptr<tds::TDSClient>(new tds::TDSClient());
        int rc;
        if (this->password.size() > 0){
          rc = db->connect(this->host, this->user, this->password);
        }
        else {
          //windows nt
          rc = db->connect(this->host, this->user);
        }
        if (rc)
          return rc;

        rc = db->useDatabase(this->database);
        if (rc)
          return rc;

        db->sql(script);

        rc = db->execute();

        spdlog::get("logger")->info() << "ssrs::rdl::generator::run() sent => " << script;

        if (rc)
          return rc;

        return 0;

      }
    private:
      std::string script;
      std::string host;
      std::string database;
      std::string user;
      std::string password;
    };

  }
}

#endif