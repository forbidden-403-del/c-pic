@echo off
setlocal EnableDelayedExpansion

rem ===== Arguments =====
rem %1 = output directory
rem %2 = target triple
rem %3 = output base name
if "%~1"=="" (
    echo Usage: %~nx0 ^<outdir^> ^<target-triple^> ^<output-name^> ^<sources and flags...^>
    exit /b 1
)

if "%~2"=="" (
    echo Error: Target triple not specified.
    exit /b 1
)

if "%~3"=="" (
    echo Error: Output name not specified.
    exit /b 1
)

set "OUTDIR=%~1"
set "TARGET=%~2"
set "OUTNAME=%~3"

rem Build ARGS = all args except first 3 (%1 %2 %3)
set "ARGS="
set "skip=3"
for %%A in (%*) do (
    if !skip! GTR 0 (
        set /a skip-=1
    ) else (
        set "ARGS=!ARGS! %%A"
    )
)

rem ===== PATH to LLVM =====
set "PATH=C:\Program Files\LLVM\bin;%PATH%"

rem ===== Output paths =====
if not exist "%OUTDIR%" mkdir "%OUTDIR%"

set "OUTPUT=%OUTDIR%\%OUTNAME%.elf"
set "BIN=%OUTDIR%\%OUTNAME%.bin"
set "DISASM=%OUTDIR%\%OUTNAME%.txt"

rem ===== Compile =====
clang -Wall -Wextra -Werror -Qn -fuse-ld=lld ^
    -target %TARGET% ^
    -nostdinc -nostartfiles -nodefaultlibs ^
    -nostdlib -fno-ident -mno-stack-arg-probe -fno-stack-check ^
    -ffunction-sections -fdata-sections ^
    -fno-asynchronous-unwind-tables ^
    -fno-unwind-tables ^
    -Wl,--gc-sections ^
    -fno-builtin ^
    -Wl,-T,linker.script,-e,main ^
    -o "%OUTPUT%" ^
    !ARGS!

if errorlevel 1 goto :end

rem ===== Tools =====
llvm-objcopy --dump-section=.text="%BIN%" "%OUTPUT%"
llvm-objdump -d -s -j .text "%OUTPUT%" > "%DISASM%"

call :filesize "%BIN%"
echo %OUTNAME%: Position-independent binary size is %size% bytes
goto :eof

:filesize
set "size=0"
for %%A in (%~1) do set "size=%%~zA"
exit /b

:end
endlocal & exit /b 1
