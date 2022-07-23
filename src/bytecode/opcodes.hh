#pragma once

namespace bytecode {

enum Opcode {
  // Debug/error ops
  kTrap = 0x00,
  kReturn = 0x01,
  kBreakpoint = 0x02,
  kPrintLong = 0x03,
  kPrintChar = 0x04,
  
  // Arithmetic operations
  kNop = 0x10,
  kAddLong = 0x11,
  kMulLong = 0x12,

  // Loading/storing
  kImmByte = 0x40,

  // Stack manipulation
  kDup = 0x60,
  kRot2 = 0x61,
  kRot3 = 0x62,
};

} // namespace bytecode
