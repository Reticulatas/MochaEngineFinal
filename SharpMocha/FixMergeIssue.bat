../PostMerge.sh
rmdir /s /q interface
mkdir interface
swigwin\swig.exe -c++ -csharp -outdir interface\ "%CD%\..\Engine\interfaces\_module.i"