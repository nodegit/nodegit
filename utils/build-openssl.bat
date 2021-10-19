@call %1 %2

echo %cd%

perl .\Configure %3 shared no-ssl2 no-ssl3 no-comp --prefix="%cd%\.." --openssldir="%cd%\.."

nmake install
