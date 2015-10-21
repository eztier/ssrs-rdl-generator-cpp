#ifndef SSRS_RDL_GENERATOR_HPP
#define SSRS_RDL_GENERATOR_HPP

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

      template<typename T>
      string compile(const shared_ptr<T> o){

        //read entire template file 
        std::ifstream file(tpl.c_str());
        std::string tplx((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        if (tplx.size() == 0){
          return "";
        }

        //compile tempplate with context to create a soap message
        Plustache::template_t t;
        std::string result = t.render(tplx, *o);

        return result;
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
      generator(std::string _host, std::string _database, std::string _user, std::string _password, std::string _script) : host(_host), database(_database), user(_user), password(_password), script(_script){}
      generator(std::string _host, std::string _database, std::string _script) : host(_host), database(_database), script(_script){}
      
      shared_ptr<Plustache::Context> generateTemplateContext(){

        std::regex re("^_+");

        auto ctx = make_shared<Plustache::Context>();
        PlustacheTypes::CollectionType c;

        //used by datasources template
        PlustacheTypes::ObjectType cmd, svr, dbs;
        cmd["command"] = script;
        svr["host"] = host;
        dbs["database"] = database;
        (*ctx).add(cmd);
        (*ctx).add(svr);
        (*ctx).add(dbs);
        
        //replace "select" with "select top 0" 
        std::regex e("select\\s");
        script = std::regex_replace(script, e, "select top 0 ");

        int rc;
        auto db = unique_ptr<tds::TDSClient>(new tds::TDSClient());
        rc = db->connect(host, user, password);
        rc = db->useDatabase(database);
        db->sql(script);
        rc = db->execute();

        /*
        int fieldcount = db->rows->fieldNames->size();
        for (int i = 0; i < fieldcount; i++){
          PlustacheTypes::ObjectType o;
          auto col = std::regex_replace(db->rows->fieldNames->at(i)->value, re, "");
          o["name"] = col;
          c.push_back(o);
        }
        */

        int fieldcount = db->rows->fieldMetas->size();
        for (int i = 0; i < fieldcount; i++){
          PlustacheTypes::ObjectType o;
          auto col = db->rows->fieldMetas->at(i)->value;
          auto colname = std::regex_replace(col.name, re, "");
          o["name"] = colname;
          o["value"] = "Fields!" + colname + ".Value";

          //determine the column type
          auto binty = tds::binaryTypes.find(col.type);
          if (binty != tds::binaryTypes.end()){
            o["value"] = "Convert.ToBase64String(" + o["value"] + ")";
          }
          auto datty = tds::dateTypes.find(col.type);
          if (datty != tds::dateTypes.end()){
            o["value"] = "CDate(" + o["value"] + ")";
          }

          o["value"] = "=" + o["value"];
          c.push_back(o);
        }

        //used by datasets, tablix-column-hierarchy, tablix-columns, tablix-rows
        ctx->add("fields", c);

        db.reset();

        return ctx;

      }

      shared_ptr<string> compile(shared_ptr<Plustache::Context> ctx){

        generator_impl<XmlElement::DataSources> datasourcesGen;
        auto datasourcesTpl = datasourcesGen.compile(ctx);
        
        generator_impl<XmlElement::DataSets> datasetsGen;
        auto datasetsTpl = datasetsGen.compile(ctx);

        generator_impl<XmlElement::TablixColumns> columnsGen;
        auto columnsTpl = columnsGen.compile(ctx);

        generator_impl<XmlElement::TablixRows> rowsGen;
        auto rowsTpl = rowsGen.compile(ctx);

        generator_impl<XmlElement::TablixColumnHierarchy> columnHierGen;
        auto columnHierTpl = columnHierGen.compile(ctx);

        generator_impl<XmlElement::TablixRowHierarchy> rowHierGen;
        auto rowHierTpl = rowHierGen.compile(ctx);

        /*
          report sections compile
        */
        auto repSecCtx = make_shared<PlustacheTypes::ObjectType>();
        (*repSecCtx)["tablixColumns"] = columnsTpl;
        (*repSecCtx)["tablixRows"] = rowsTpl;
        (*repSecCtx)["tablixColumnHierarchy"] = columnHierTpl;
        (*repSecCtx)["tablixRowHierarchy"] = rowHierTpl;

        generator_impl<XmlElement::ReportSections> repSecGen;
        auto repSecTpl = repSecGen.compile(repSecCtx);

        /*
          root compile
        */
        auto rootCtx = make_shared<PlustacheTypes::ObjectType>();
        (*rootCtx)["dataSources"] = datasourcesTpl;
        (*rootCtx)["dataSets"] = datasetsTpl;
        (*rootCtx)["reportSections"] = repSecTpl;
        generator_impl<XmlElement::Root> rootGen;
        auto rootTpl = rootGen.compile(rootCtx);

        auto r = make_shared<string>(rootTpl);

        return r;
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