set(LLVM_LINK_COMPONENTS Support TableGen)

add_llvm_tool(llvm-othergen
  SSHConfigEmitter.cpp
  TableGen.cpp
  )
