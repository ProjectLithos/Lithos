# OESDK freestanding kernel

Build the project with **Build > Build Solution**. Start it with **Debug >
Start Without Debugging**. Debug builds send `kdebugf` messages to QEMU's COM1
serial console. `kprintf` always writes to the guest VGA text screen.
