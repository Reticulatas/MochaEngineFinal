echo Invoking SWIG...
echo on
..\Engine\interfaces\MochaDirCheck.py "%CD%\\..\\Engine\\interfaces\\" "i"
set ret=%ERRORLEVEL%
if %ret% == 1 (
rmdir /s /q interface
mkdir interface
swigwin\swig.exe -c++ -csharp -namespace MochaInterface -outdir interface\ "%CD%\..\Engine\interfaces\_module.i"
)
if %ret% == 0 (echo No Changes - Swig Skipped)