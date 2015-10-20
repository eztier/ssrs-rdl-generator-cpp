ssrs-rdl-generator-cpp
======================

####A fast and versatile tool to dynamically generate SQL Server Reporting Services RDL files in C++11
Just supply it any valid SQL statement and it'll do the rest

###Dependencies
* [plustache](https://github.com/mrtazz/plustache) {{mustaches}} for C++
* [freetds](https://github.com/FreeTDS/freetds) Free implementation of Sybase's DB-Library, CT-Library
* [spdlog](https://github.com/gabime/spdlog) Extremely fast logger
* [boost](http://sourceforge.net/projects/boost/files/boost-binaries/) For ```boost/filesystem.hpp```

###Notes  
__1.__ For Chrome/Safari to work, add the following to ```C:\Program Files\Microsoft SQL Server\MSRS10_50.MSSQLSERVER\Reporting Services\ReportManager\js\ReportingServices.js```
  ```js
  function pageLoad() {
    var element = document.getElementById("ctl31_ctl10");
    if (element) {
      element.style.overflow = "visible";
    }
  }
  ```
__2.__ Use branch "dollar-sign" of [plustache](https://github.com/chesles/plustache.git) because of issue [#20](https://github.com/mrtazz/plustache/pull/20/files)
"suppress handling of special characters in replacement text", ie missing parenthesis post-rendering

