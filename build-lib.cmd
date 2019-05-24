git submodule update --init

powershell -executionpolicy bypass lib\poco\buildwin.ps1^
 -poco_base lib\poco^
 -vs_version 140^
 -linkmode all^
 -config both^
 -platform x64^
 -omit "'CppUnit;CppUnit/WinTestRunner;Encodings;Crypto;NetSSL_OpenSSL;Data;Data/SQLite;Data/ODBC;Data/MySQL;Zip;PageCompiler;PageCompiler/File2Page;PDF;CppParser;MongoDB;Redis;PocoDoc;ProGen'"

call build-lua.cmd
