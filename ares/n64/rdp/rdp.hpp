//Reality Display Processor

struct RDP : Thread, Memory::RCP<RDP> {
  Node::Object node;

  struct Debugger {
    auto load(Node::Object) -> void;
    auto command(string_view) -> void;
    auto ioDPC(bool mode, u32 address, u32 data) -> void;
    auto ioDPS(bool mode, u32 address, u32 data) -> void;

    struct Tracer {
      Node::Debugger::Tracer::Notification command;
      Node::Debugger::Tracer::Notification io;
    } tracer;
  } debugger;

  auto load(Node::Object) -> void;
  auto unload() -> void;
  auto main() -> void;
  auto power(bool reset) -> void;
  auto crash(const char* reason) -> void;

  auto rendererLoad() -> bool;
  auto rendererUnload() -> void;
  auto rendererProcess() -> bool;
  auto rendererScanout(const VI::Registers&) -> bool;
  auto rendererMapScanout(const u8*& rgba, u32& width, u32& height, bool& interlaced, bool& field) -> void;
  auto rendererSerialize(serializer&) -> void;

  struct Renderer;
  Renderer* renderer = nullptr;
  bool rendererThreaded = true;
  u32 rendererScale = 1;
  bool disableVIDitherFilter = false;
  bool disableVIDivotFilter = false;
  bool disableVIGammaDither = false;
  bool disableVIAntiAliasing = false;

  auto render() -> void;
  auto syncFull() -> void;

  auto readWord(u32 address, Thread& thread) -> u32;
  auto writeWord(u32 address, u32 data, Thread& thread) -> void;
  auto flushCommands() -> void;
  auto serialize(serializer&) -> void;

  struct Command {
    n24 start;
    n24 end;
    n24 current;
    n24 clock;
    n24 bufferBusy;
    n24 pipeBusy;
    n24 tmemBusy;
    n1 source;  //0 = RDRAM, 1 = DMEM
    n1 freeze;
    n1 crashed;
    n1 flush;
    n1 startValid;
    n1 endValid;
    n1 startGclk;
    n1 ready = 1;
  } command;

  struct IO : Memory::RCP<IO> {
    RDP& self;
    IO(RDP& self) : self(self) {}

    auto readWord(u32 address, Thread& thread) -> u32;
    auto writeWord(u32 address, u32 data, Thread& thread) -> void;

    struct BIST {
      n1 check;
      n1 go;
      n1 done;
      n8 fail;
    } bist;

    struct Test {
      n1 enable;
      n7 address;
      array<u32[128]> data;
    } test;
  } io{*this};

  n1 mapIdentityWarned;
};

extern RDP rdp;
