set CC=gcc -static

if "%1"=="tcc" (set CC=.\tcc\tcc.exe)

%CC% copy_stages.c -o ..\Data\plugins\copy_stages.exe
%CC% rand_osc_levels.c -o ..\Data\plugins\rand_osc_levels.exe
%CC% linearize.c -o ..\Data\plugins\linearize.exe