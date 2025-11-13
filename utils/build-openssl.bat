rem Build OpenSSL for Windows
rem %1 - path to vcvarsall.bat
rem %2 - architecture argument for vcvarsall.bat
rem %3 - OpenSSL Configure target

@call %1 %2

perl .\Configure %3 no-shared no-ssl2 no-ssl3 no-comp --prefix="%cd%\.." --openssldir="%cd%\.." || goto :error

nmake || goto :error

if "%NODEGIT_SKIP_TESTS%" NEQ "1" (
  nmake test || goto :error
)

nmake install || goto :error

goto :EOF

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%