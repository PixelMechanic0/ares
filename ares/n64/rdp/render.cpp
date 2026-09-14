auto RDP::render() -> void {
  if(!rendererProcess()) crash("software renderer could not process the RDP command list");
}

auto RDP::syncFull() -> void {
  if(!command.crashed) {
    mi.raise(MI::IRQ::DP);
    command.bufferBusy = 0;
    command.pipeBusy = 0;
  }
  command.startGclk = 0;
}
