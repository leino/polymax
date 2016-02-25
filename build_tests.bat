@echo off

REM === constants =======
set buildtype_release=0
set buildtype_internal=1
set debuglevel_expensive_checks=1
set performance_spam_level=0
set source_path=%1
set builds_path=%2

REM === environment =======
set vc_install_path="C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC"

REM === build target =======
set target_buildtype=%buildtype_internal%
set target_debuglevel=%debuglevel_expensive_checks%
set target_architecture=x64


REM === warnings ======
REM this is disabled because potentially unsafe things are totally fine
set disable_potentially_unsafe_methods_warning_flag=/wd4996
REM this is disabled because I cannot set stuff to positive float infinity if this is on
set disable_overflow_in_constant_arithmetic_warning_flag=/wd4756
REM this is disabled because I like to do while(1) loops
set disable_conditional_expression_is_constant_warning_flag=/wd4127

set disabled_warning_flags=^
    %disable_potentially_unsafe_methods_warning_flag%^
    %disable_overflow_in_constant_arithmetic_warning_flag%^
    %disable_conditional_expression_is_constant_warning_flag%

set common_compiler_flags=^
    /nologo^
    /MT^
    /Gm-^
    /EHa-^
    /Od^
    /Oi^
    /WX^
    /W4^
    %disabled_warning_flags%

set common_linker_flags=/incremental:no

set output_switches=^
    /Fo%builds_path%\^
    /Fe%builds_path%\^
    /Fd%builds_path%\
    
set buildtype_def=/DQUI_BUILDTYPE=%target_buildtype%
set debuglevel_def=/DQUI_DEBUGLEVEL=%target_debuglevel%

set defs=^
    /DQUI_BUILDTYPE_RELEASE=%buildtype_release%^
    /DQUI_BUILDTYPE_INTERNAL=%buildtype_internal%^
    /DQUI_PERFORMANCE_SPAM_LEVEL=%performance_spam_level%^
    /DQUI_DEBUGLEVEL_EXPENSIVE_CHECKS=%debuglevel_expensive_checks%
    
REM make sure that the output direcotry exists
IF NOT EXIST %builds_path% mkdir %builds_path%

REM set the compiler environment variables for 64 bit builds
call %vc_install_path%\"\vcvarsall.bat" %target_architecture%

set compile_command=^
    cl^
    %common_compiler_flags%^
    %output_switches%^
    /Z7^
    %libs%^
    %defs%^
    %buildtype_def%^
    %debuglevel_def%

call %compile_command%^
     %source_path%\buffer_test.cpp^
     /link %common_linker_flags%^
     /OUT:%builds_path%\buffer_test.exe

REM exit early if compile failed
if %ERRORLEVEL% gtr 0 (
    exit /b %ERRORLEVEL%
    )
