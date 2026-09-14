ares_components(sm5k)

ares_add_sources(
  CORE #
    n64
  UNITY #
    n64.cpp
  PRIMARY #
    memory/memory.cpp
    system/system.cpp
    cartridge/cartridge.cpp
    cic/cic.cpp
    controller/controller.cpp
    dd/dd.cpp
    mi/mi.cpp
    vi/vi.cpp
    ai/ai.cpp
    pi/pi.cpp
    pif/pif.cpp
    ri/ri.cpp
    si/si.cpp
    rdram/rdram.cpp
    cpu/cpu.cpp
    rsp/rsp.cpp
    rdp/rdp.cpp
    aleck64/aleck64.cpp
)

target_enable_feature(ares "N64 SoftRDP renderer")

# The N64 core currently targets AVX2-capable x86-64 hosts. Keep
# this requirement at the component boundary so every N64 translation unit has
# the same instruction-set contract as the rasterizer.
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" _n64_system_processor)
if(NOT arch STREQUAL "x64" AND NOT _n64_system_processor MATCHES "^(x86_64|amd64|x64)$")
  message(FATAL_ERROR "The N64 core currently requires an x86-64 processor with AVX2 support.")
endif()
if(MSVC)
  target_compile_options(ares PRIVATE /arch:AVX2)
else()
  target_compile_options(ares PRIVATE -mavx2)
endif()

if(OS_WINDOWS)
  include(cmake/os-windows.cmake)
elseif(OS_MACOS)
  include(cmake/os-macos.cmake)
elseif(OS_LINUX OR OS_FREEBSD OR OS_OPENBSD)
  include(cmake/os-linux.cmake)
endif()

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    n64.hpp
    accuracy.hpp
    CMakeLists.txt
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    ai/ai.hpp
    ai/debugger.cpp
    ai/io.cpp
    ai/serialization.cpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    aleck64/aleck64.hpp
    aleck64/controls.cpp
    aleck64/debugger.cpp
    aleck64/serialization.cpp
    aleck64/io.cpp
    aleck64/vdp.cpp
    aleck64/game-config/11beat.hpp
    aleck64/game-config/doncdoon.hpp
    aleck64/game-config/kurufev.hpp
    aleck64/game-config/mayjin3.hpp
    aleck64/game-config/starsldr.hpp
    aleck64/game-config/twrshaft.hpp
    aleck64/game-config/vivdolls.hpp
    aleck64/game-config/hipai.hpp
    aleck64/game-config/hipai2.hpp
    aleck64/game-config/srmvs.hpp
    aleck64/game-config/mtetrisc.hpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    cartridge/cartridge.hpp
    cartridge/debugger.cpp
    cartridge/flash.cpp
    cartridge/isviewer.cpp
    cartridge/joybus.cpp
    cartridge/rtc.cpp
    cartridge/serialization.cpp
    cartridge/slot.cpp
    cartridge/slot.hpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    cic/cic.hpp
    cic/commands.cpp
    cic/io.cpp
    cic/serialization.cpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    controller/controller.hpp
    controller/port.cpp
    controller/port.hpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    controller/aleck64/aleck64.cpp
    controller/aleck64/aleck64.hpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    controller/gamepad/bio-sensor.cpp
    controller/gamepad/gamepad.cpp
    controller/gamepad/gamepad.hpp
    controller/gamepad/transfer-pak.cpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    controller/gamepad-gcn/gamepad-gcn.cpp
    controller/gamepad-gcn/gamepad-gcn.hpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    controller/mouse/mouse.cpp
    controller/mouse/mouse.hpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    cpu/algorithms.cpp
    cpu/context.cpp
    cpu/cpu.hpp
    cpu/dcache.cpp
    cpu/debugger.cpp
    cpu/disassembler.cpp
    cpu/exceptions.cpp
    cpu/interpreter-cop2.cpp
    cpu/interpreter-fpu.cpp
    cpu/interpreter-ipu.cpp
    cpu/interpreter-scc.cpp
    cpu/interpreter.cpp
    cpu/memory.cpp
    cpu/recompiler.cpp
    cpu/recompiler-fpu.cpp
    cpu/recompiler-ipu.cpp
    cpu/serialization.cpp
    cpu/tlb.cpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    dd/controller.cpp
    dd/dd.hpp
    dd/debugger.cpp
    dd/drive.cpp
    dd/io.cpp
    dd/rtc.cpp
    dd/serialization.cpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    memory/bus.hpp
    memory/io.hpp
    memory/memory.hpp
    memory/lsb/readable.hpp
    memory/lsb/writable.hpp
    memory/msb/readable.hpp
    memory/msb/writable.hpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    mi/debugger.cpp
    mi/io.cpp
    mi/mi.hpp
    mi/serialization.cpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    pi/bus.hpp
    pi/debugger.cpp
    pi/device.hpp
    pi/dma.cpp
    pi/io.cpp
    pi/pi.hpp
    pi/serialization.cpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    pif/debugger.cpp
    pif/hle.cpp
    pif/io.cpp
    pif/pif.hpp
    pif/serialization.cpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    rdp/debugger.cpp
    rdp/io.cpp
    rdp/rdp.hpp
    rdp/render.cpp
    rdp/serialization.cpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    rdram/debugger.cpp
    rdram/io.cpp
    rdram/rdram.hpp
    rdram/serialization.cpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    ri/debugger.cpp
    ri/io.cpp
    ri/ri.hpp
    ri/serialization.cpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    rsp/debugger.cpp
    rsp/decoder.cpp
    rsp/disassembler.cpp
    rsp/dma.cpp
    rsp/interpreter-ipu.cpp
    rsp/interpreter-scc.cpp
    rsp/interpreter-vpu.cpp
    rsp/interpreter.cpp
    rsp/io.cpp
    rsp/recompiler.cpp
    rsp/rsp.hpp
    rsp/serialization.cpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    si/debugger.cpp
    si/dma.cpp
    si/io.cpp
    si/serialization.cpp
    si/si.hpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    system/serialization.cpp
    system/system.hpp
)

ares_add_sources(
  CORE #
    n64
  INCLUDED #
    vi/debugger.cpp
    vi/io.cpp
    vi/serialization.cpp
    vi/vi.hpp
)

include(cmake/softrdp.cmake)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(ares PUBLIC Threads::Threads)

if(OS_WINDOWS)
  target_link_libraries(ares PRIVATE winmm)
else()
  target_link_libraries(ares PRIVATE dl)
endif()
