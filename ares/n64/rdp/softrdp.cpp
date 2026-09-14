#include <n64/n64.hpp>

extern "C" {
  #include <core/sr.h>
  #include <core/vi.h>  //VI_MAX_OUTPUT_WIDTH/HEIGHT, for sizing the scanout buffer
}

namespace ares::Nintendo64 {

struct RDP::Renderer {
  sr_context* context = nullptr;
  sr_host_interface host = {};

  u32 dpRegs[SR_DP_REGISTER_COUNT] = {};
  u32 viRegs[SR_VI_REGISTER_COUNT] = {};
  u32 miIntr = 0;  //softrdp ORs DP_INTERRUPT here; ares raises the real IRQ in the callback

  std::vector<sr_rgba8> pixels;
  u32 width = 0;
  u32 height = 0;
  bool interlaced = false;
  bool field = false;
  bool valid = false;

};

static auto raiseDPInterrupt(void*) -> void {
  //called synchronously from within sr_process_rdp_list on the emulation
  //thread, never from a worker, so touching ares state here is safe.
  rdp.syncFull();
}

static auto traceRDPCommand(void* userdata, u32, u32 id, const u32* words, u32 count) -> void {
  static const char* names[64] = {
    "No_Operation", "Invalid_01", "Invalid_02", "Invalid_03",
    "Invalid_04", "Invalid_05", "Invalid_06", "Invalid_07",
    "Unshaded_Triangle", "Unshaded_Zbuffer_Triangle", "Texture_Triangle", "Texture_Zbuffer_Triangle",
    "Shaded_Triangle", "Shaded_Zbuffer_Triangle", "Shaded_Texture_Triangle", "Shaded_Texture_Zbuffer_Triangle",
    "Invalid_10", "Invalid_11", "Invalid_12", "Invalid_13",
    "Invalid_14", "Invalid_15", "Invalid_16", "Invalid_17",
    "Invalid_18", "Invalid_19", "Invalid_1a", "Invalid_1b",
    "Invalid_1c", "Invalid_1d", "Invalid_1e", "Invalid_1f",
    "Invalid_20", "Invalid_21", "Invalid_22", "Invalid_23",
    "Texture_Rectangle", "Texture_Rectangle_Flip", "Sync_Load", "Sync_Pipe",
    "Sync_Tile", "Sync_Full", "Set_Key_GB", "Set_Key_R",
    "Set_Convert", "Set_Scissor", "Set_Primitive_Depth", "Set_Other_Modes",
    "Load_Texture_LUT", "Invalid_31", "Set_Tile_Size", "Load_Block",
    "Load_Tile", "Set_Tile", "Fill_Rectangle", "Set_Fill_Color",
    "Set_Fog_Color", "Set_Blend_Color", "Set_Primitive_Color", "Set_Environment_Color",
    "Set_Combine_Mode", "Set_Texture_Image", "Set_Mask_Image", "Set_Color_Image",
  };

  auto self = (RDP*)userdata;
  if(!self || !self->debugger.tracer.command->enabled()) return;
  u64 opcode = count > 0 ? (u64)words[0] << 32 : 0;
  if(count > 1) opcode |= words[1];
  string message{hex(opcode, 16L), "  ", names[id & 63]};
  self->debugger.command(message);
}

auto RDP::rendererLoad() -> bool {
  rendererUnload();
  renderer = new RDP::Renderer;
  auto& impl = *renderer;

  if(rendererScale != 1 && rendererScale != 2) rendererScale = 1;

  impl.host.scale = rendererScale;
  impl.host.workers = rendererThreaded ? 0 : 1;
  impl.host.disable_vi_dither_filter = disableVIDitherFilter;
  impl.host.disable_vi_divot_filter = disableVIDivotFilter;
  impl.host.disable_vi_gamma_dither = disableVIGammaDither;
  impl.host.disable_vi_aa = disableVIAntiAliasing;
  impl.host.rdram = rdram.ram.data;
  impl.host.rdram_size = rdram.ram.size;
  impl.host.hidden_rdram = rdram.hidden.data;
  impl.host.hidden_rdram_size = rdram.hidden.size;
  impl.host.dmem = rsp.dmem.data;
  impl.host.mi_intr_reg = &impl.miIntr;
  impl.host.raise_mi_interrupt = &raiseDPInterrupt;
  impl.host.trace_rdp_command = &traceRDPCommand;
  impl.host.userdata = this;

  for(u32 n : range(SR_DP_REGISTER_COUNT)) impl.host.dp_regs[n] = &impl.dpRegs[n];
  for(u32 n : range(SR_VI_REGISTER_COUNT)) impl.host.vi_regs[n] = &impl.viRegs[n];

  //the scanout image is at most the VI's output limits, scaled
  impl.pixels.resize((size_t)VI_MAX_OUTPUT_WIDTH * rendererScale * VI_MAX_OUTPUT_HEIGHT * rendererScale);

  impl.context = sr_create(&impl.host);
  if(!impl.context) {
    delete renderer;
    renderer = nullptr;
    platform->status("RDP initialization failed");
    return false;
  }

  platform->status("RDP enabled: software rendering");
  return true;
}

auto RDP::rendererUnload() -> void {
  if(renderer) {
    if(renderer->context) sr_destroy(renderer->context);
    delete renderer;
    renderer = nullptr;
  }
}

auto RDP::rendererProcess() -> bool {
  if(!renderer || !renderer->context) return false;
  auto& impl = *renderer;
  auto& command = rdp.command;

  impl.dpRegs[SR_DP_START]   = command.start;
  impl.dpRegs[SR_DP_END]     = command.end;
  impl.dpRegs[SR_DP_CURRENT] = command.current;
  impl.dpRegs[SR_DP_STATUS]  = command.source ? 1 : 0;  //bit 0 = XBUS_DMA

  auto result = sr_process_rdp_list(impl.context);
  if(result != SR_OK) {
    const char* reason = "software renderer rejected an RDP command list";
    if(result == SR_ERROR_INVALID_ARGUMENT) reason = "software renderer received invalid RDP state";
    if(result == SR_ERROR_BAD_COMMAND) reason = "software renderer received a malformed RDP command";
    if(result == SR_ERROR_UNSUPPORTED) reason = "software renderer encountered an unsupported RDP command";
    crash(reason);
  }

  //softrdp acknowledges the list by pointing START and CURRENT at END
  command.start   = impl.dpRegs[SR_DP_START];
  command.current = impl.dpRegs[SR_DP_CURRENT];
  return true;
}

auto RDP::rendererScanout(const VI::Registers& registers) -> bool {
  if(!renderer || !renderer->context) return false;
  auto& impl = *renderer;

  impl.viRegs[SR_VI_STATUS] = registers.status;
  impl.viRegs[SR_VI_ORIGIN] = registers.origin;
  impl.viRegs[SR_VI_WIDTH] = registers.width;
  impl.viRegs[SR_VI_INTR] = registers.intr;
  impl.viRegs[SR_VI_CURRENT] = registers.current;
  impl.viRegs[SR_VI_TIMING] = registers.timing;
  impl.viRegs[SR_VI_V_SYNC] = registers.vSync;
  impl.viRegs[SR_VI_H_SYNC] = registers.hSync;
  impl.viRegs[SR_VI_LEAP] = registers.leap;
  impl.viRegs[SR_VI_H_START] = registers.hStart;
  impl.viRegs[SR_VI_V_START] = registers.vStart;
  impl.viRegs[SR_VI_V_BURST] = registers.vBurst;
  impl.viRegs[SR_VI_X_SCALE] = registers.xScale;
  impl.viRegs[SR_VI_Y_SCALE] = registers.yScale;
  impl.valid = false;

  sr_vi_frame_info info = {};
  if(sr_get_vi_frame_info(impl.context, &info) != SR_OK) return false;
  if(!info.display) {
    //a hold is "keep showing the last frame", not "show black"
    return info.hold && impl.width && impl.height;
  }

  sr_framebuffer out = {};
  out.pixels = impl.pixels.data();
  out.width = 0;  //accept the VI's own width
  out.height = info.height;
  out.stride_pixels = info.width;
  if(sr_update_screen(impl.context, &out) != SR_OK || !out.valid) return false;

  impl.width = out.width;
  impl.height = out.height;
  impl.interlaced = info.interlaced;
  impl.field = info.field;
  impl.valid = true;
  return true;
}

auto RDP::rendererMapScanout(const u8*& rgba, u32& width, u32& height, bool& interlaced, bool& field) -> void {
  if(!renderer || !renderer->width) return;
  rgba = (const u8*)renderer->pixels.data();
  width = renderer->width;
  height = renderer->height;
  interlaced = renderer->interlaced;
  field = renderer->field;
}

auto RDP::rendererSerialize(serializer& s) -> void {
  if(!renderer || !renderer->context) return;

  auto size = sr_state_snapshot_size();
  std::vector<u8> state(size);
  if(s.writing()) {
    sr_flush(renderer->context);
    if(sr_save_state(renderer->context, state.data(), state.size()) != SR_OK) {
      crash("could not save software renderer state");
      return;
    }
  }

  for(auto& byte : state) s(byte);

  if(s.reading()) {
    if(sr_load_state(renderer->context, state.data(), state.size()) != SR_OK) {
      crash("could not restore software renderer state");
    }
  }
}

}
