@echo off

REM === constants =======
set source_path=%1
set builds_path=%2
set build_type_internal=1
set build_type_release=0

REM === environment =======
set vc_install_path="C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC"

REM === build target =======
set target_architecture=x64


REM == build configuration ====
set build_type=%build_type_internal%
set debug_level=1
set debug_clear_level=0

REM == defines ====
set build_type_internal_def=/DPOLYROOT_BUILD_TYPE_INTERNAL=%build_type_internal%
set build_type_release_def=/DPOLYROOT_BUILD_TYPE_RELEASE=%build_type_release%
set build_type_def=/DPOLYROOT_BUILD_TYPE=%build_type%
set debug_clear_level_def=/DPOLYROOT_DEBUG_CLEAR_LEVEL=%debug_clear_level%
set debug_level_def=/DPOLYROOT_DEBUG_LEVEL=%debug_level%
set polyroot_defs=^
    %build_type_internal_def%^
    %build_type_release_def%^
    %build_type_def%^
    %debug_level_def%^
    %debug_clear_level_def%

set polyroot_test_defs=^
    %polyroot_defs%

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
    
set libs=^
    user32.lib^
    winmm.lib^
    d3d12.lib^
    D3DCompiler.lib^
    dxgi.lib

REM make sure that the output direcotry exists
IF NOT EXIST %builds_path% mkdir %builds_path%

REM set the compiler environment variables for 64 bit builds
call %vc_install_path%\"\vcvarsall.bat" %target_architecture%

set debug_flag=/Z7

REM NOTE: check if executable is locked
2>nul (
  >>%builds_path%\polyroot_test.exe (call )
) && (set exe_locked=0) || (set exe_locked=1)


call cl^
     %polyroot_test_defs%^
     %common_compiler_flags%^
     %output_switches%^
     %debug_flag%^
     %libs%^
     %source_path%\polyroot_test.cpp^
     /link^
     %common_linker_flags%^
     /OUT:%builds_path%\polyroot_test.exe

REM exit if compile failed
if %ERRORLEVEL% gtr 0 (
   exit /b %ERRORLEVEL%
)
