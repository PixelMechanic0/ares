# SoftRDP renderer integration.
#
# The core is compiled ONCE PER SCALE with the scale as a literal, and every
# exported symbol renamed by sr_namespace.h so both copies link into one binary.
# See src/core/sr_dispatch.c for why. Two object libraries, one per scale, is
# the CMake shape of that; a single target cannot carry two sets of flags.

set(_softrdp_dir "${CMAKE_CURRENT_SOURCE_DIR}/rdp/softrdp")

# Compiled once per scale, then namespaced.
set(
  _softrdp_scaled_srcs
  sr.c
  rdp_commands.c
  rdp_memory.c
  framebuffer.c
  raster.c
  tmem.c
  primitive.c
  kernel.c
  blend_divider.c
  vi.c
)
list(TRANSFORM _softrdp_scaled_srcs PREPEND "${_softrdp_dir}/src/core/")

# Compiled once, shared by both copies: the scale selector (the public sr_* API)
# and the thread pool, which is process-wide and has no scale dependency.
if(OS_WINDOWS)
  set(_softrdp_platform_srcs
    "${_softrdp_dir}/src/core/sr_threads_win32.c"
    "${_softrdp_dir}/src/core/sr_threads_vi.c"
    "${_softrdp_dir}/src/core/sr_async.c"
  )
else()
  set(_softrdp_platform_srcs
    "${_softrdp_dir}/src/core/sr_threads_posix.c"
    "${_softrdp_dir}/src/core/sr_threads_vi.c"
    "${_softrdp_dir}/src/core/sr_async.c"
  )
endif()

# src/present/ is deliberately absent: it is softrdp's OpenGL presenter, and
# ares presents through its own ruby video drivers instead. Dropping it is what
# keeps this integration free of any graphics API dependency.

function(_softrdp_add_scale_library target scale suffix)
  add_library(${target} OBJECT ${_softrdp_scaled_srcs})
  target_include_directories(${target} PRIVATE "${_softrdp_dir}/src")
  target_compile_definitions(
    ${target}
    PRIVATE SOFTRDP_SCALE=${scale} SR_SCALE_SUFFIX=${suffix} SOFTRDP_ENABLE_LOG=0 SOFTRDP_CENSUS=0
  )
  # Force-included so no source file has to mention the namespacing header.
  if(MSVC)
    target_compile_options(${target} PRIVATE "/FI${_softrdp_dir}/src/core/sr_namespace.h" /arch:AVX2)
  else()
    target_compile_options(${target} PRIVATE -include "${_softrdp_dir}/src/core/sr_namespace.h" -mavx2)
  endif()
  set_target_properties(${target} PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED ON POSITION_INDEPENDENT_CODE ON)
endfunction()

_softrdp_add_scale_library(softrdp_s1 1 _s1)
_softrdp_add_scale_library(softrdp_s2 2 _s2)

add_library(softrdp_shared OBJECT "${_softrdp_dir}/src/core/sr_dispatch.c" ${_softrdp_platform_srcs})
target_include_directories(softrdp_shared PRIVATE "${_softrdp_dir}/src")
target_compile_definitions(softrdp_shared PRIVATE SOFTRDP_ENABLE_LOG=0 SOFTRDP_CENSUS=0)
if(MSVC)
  target_compile_options(softrdp_shared PRIVATE /arch:AVX2)
else()
  target_compile_options(softrdp_shared PRIVATE -mavx2)
endif()
set_target_properties(
  softrdp_shared
  PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED ON POSITION_INDEPENDENT_CODE ON
)

target_sources(
  ares
  PRIVATE
    $<TARGET_OBJECTS:softrdp_s1>
    $<TARGET_OBJECTS:softrdp_s2>
    $<TARGET_OBJECTS:softrdp_shared>
)
target_include_directories(ares PRIVATE "${_softrdp_dir}/src")

# The implementation is owned by the architectural RDP component.
ares_add_sources(
  CORE #
    n64
  PRIMARY #
    rdp/softrdp.cpp
)
