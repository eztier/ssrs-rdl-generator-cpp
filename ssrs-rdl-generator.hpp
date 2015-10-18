#ifndef ALLENTOWN_HPP
#define ALLENTOWN_HPP

#include <iostream>
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
    class generator : public base<xmlelement>{};

    /*
    DataSources
    */
    template<>
    class generator<XmlElement::Root> : public base<XmlElement::Root>{
    public:
      generator(){ tpl = "tpl/root"; }

    };

    /*
      DataSources
    */
    template<>
    class generator<XmlElement::DataSources> : public base<XmlElement::DataSources>{
    public:
      generator(){ tpl = "tpl/datasources"; }

    };

    /*
      DataSets
    */
    template<>
    class generator<XmlElement::DataSets> : public base<XmlElement::DataSets>{
    public:
      generator(){ tpl = "tpl/datasets"; }

    };

    /*
      ReportSections
    */
    template<>
    class generator<XmlElement::ReportSections> : public base<XmlElement::ReportSections>{
    public:
      generator(){ tpl = "tpl/report-sections"; }

    };

    /*
      TablixColumns
    */
    template<>
    class generator<XmlElement::TablixColumns> : public base<XmlElement::TablixColumns>{
    public:
      generator(){ tpl = "tpl/tablix-columns"; }

    };

    /*
      TablixRows
    */
    template<>
    class generator<XmlElement::TablixRows> : public base<XmlElement::TablixRows>{
    public:
      generator(){ tpl = "tpl/tablix-rows"; }

    };

    /*
      TablixColumnHierarchy
    */
    template<>
    class generator<XmlElement::TablixColumnHierarchy> : public base<XmlElement::TablixColumnHierarchy>{
    public:
      generator(){ tpl = "tpl/tablix-column-hierarchy"; }

    };

    /*
      TablixRowHierarchy
    */
    template<>
    class generator<XmlElement::TablixRowHierarchy> : public base<XmlElement::TablixRowHierarchy>{
    public:
      generator(){ tpl = "tpl/tablix-row-hierarchy"; }

    };

  }
}

#endif