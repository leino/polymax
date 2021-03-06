@echo off

REM === constants =======
set source_path=%1
set builds_path=%2
set build_type=%3

REM === environment =======
set vc_install_path="C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC"

REM === build target =======
set target_architecture=x64


REM === warnings ======
REM this is disabled because potentially unsafe things are totally fine
set disable_potentially_unsafe_methods_warning_flag=/wd4996
REM this is disabled because I cannot set stuff to positive float infinity if this is on
set disable_overflow_in_constant_arithmetic_warning_flag=/wd4756
REM this is disabled because I like to do while(1) loops
set disable_conditional_expression_is_constant_warning_flag=/wd4127
REM unalignedness is fine by me on the platforms I target for now
set disable_potential_unaligned_address_taking_flag=/wd4366
REM assignment in conditionals is fine
set disable_assignment_within_conditionals_warning=/wd4706
set disabled_warning_flags=^
    %disable_potentially_unsafe_methods_warning_flag%^
    %disable_overflow_in_constant_arithmetic_warning_flag%^
    %disable_conditional_expression_is_constant_warning_flag%^
    %disable_potential_unaligned_address_taking_flag%^
    %disable_assignment_within_conditionals_warning%

set common_compiler_flags=^
    /nologo^
    /Gm-^
    /EHa-^
    /WX^
    /W4^
    %disabled_warning_flags%

set common_linker_flags=/incremental:no

set output_switches=^
    /Fo%builds_path%\^
    /Fe%builds_path%\^
    /Fd%builds_path%\
    
REM make sure that the output direcotry exists
IF NOT EXIST %builds_path% mkdir %builds_path%

REM set the compiler environment variables for amd64 architecture
call %vc_install_path%\"\vcvarsall.bat" %target_architecture%

if %build_type% == debug (
set runtime_library_flags=/MDd
)
if %build_type% == release (
set runtime_library_flags=/MD
)

if %build_type% == debug (
set debug_flags=/Zi
)
if %build_type% == release (
set debug_flags=
)

if %build_type% == debug (
set optimization_flags=/Od /Oi
)
if %build_type% == release (
set optimization_flags=/O2 /Oi
)

if %build_type% == debug (
set define_flags=/DENSURE_DEBUGBREAK_ON_ERROR
)
if %build_type% == release (
set define_flags=
)

call cl^
     %common_compiler_flags%^
     %runtime_library_flags%^
     %output_switches%^
     %define_flags%^
     %debug_flags%^
     %optimization_flags%^
     %source_path%\analysis.cpp^
     /link^
     %common_linker_flags%^
     /OUT:%builds_path%\test_%build_type%.exe

REM exit if compile failed
if %ERRORLEVEL% gtr 0 (
   exit /b %ERRORLEVEL%
)
