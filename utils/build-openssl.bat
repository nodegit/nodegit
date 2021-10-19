@call "%1"

echo %cd%

perl .\Configure %2 shared no-ssl2 no-ssl3 no-comp --prefix="%cd%\.." --openssldir="%cd%\.."

nmake install
