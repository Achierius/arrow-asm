#pragma once

namespace bytecode {

enum Opcode {
  // Debug/error ops
  kTrap = 0x00,
  kReturn = 0x01,
  kBreakpoint = 0x02,
  kPrintLong = 0x03,
  kPrintChar = 0x04,
  
  // Binary Arithmetic operations
  kNop = 0x10,
  kAddLong = 0x11,
  kSubLong = 0x12,
  kMulLong = 0x13,
  kIDivLong = 0x14,    // pushes TOS1 / TOS
  kModuloLong = 0x15,  // pushes TOS1 % TOS
  kLeftShiftLong = 0x16, // pushes TOS1 << TOS
  kRightShiftLogicalLong = 0x17, // pushes TOS1 >> TOS
  kRightShiftArithmeticLong = 0x18, // pushes TOS1 >>> TOS

  // Loading/storing
  kImmByte = 0x40,
  kConstant = 0x41,
  kBiasConstantWindow = 0x42,

  // Stack manipulation
  kDup = 0x60,
  kRot2 = 0x61,
  kRot3 = 0x62,
};

} // namespace bytecode
