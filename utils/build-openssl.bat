@call %1 %2

perl .\Configure %3 no-shared no-ssl2 no-ssl3 no-comp --prefix="%cd%\.." --openssldir="%cd%\.." || goto :error

nmake || goto :error
nmake test || goto :error
nmake install || goto :error

goto :EOF

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%