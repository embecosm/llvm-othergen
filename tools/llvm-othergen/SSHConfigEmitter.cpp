//===- SSHConfigEmitter.cpp - Generate SSH Configurations -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This tablegen backend is responsible for emitting SSH configs.
//
//===----------------------------------------------------------------------===//

#include "TableGenBackends.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <cassert>
using namespace llvm;

static void emitSSHHeader(raw_ostream &OS) {
  OS << "######################################\n";
  OS << "#         mmmm   mmmm  m    m        #\n";
  OS << "#        #\"   \" #\"   \" #    #        #\n";
  OS << "#        \"#mmm  \"#mmm  #mmmm#        #\n";
  OS << "#            \"#     \"# #    #        #\n";
  OS << "#        \"mmm#\" \"mmm#\" #    #        #\n";
  OS << "#                                    #\n";
  OS << "#   Simon Cook's Global SSH Config   #\n";
  OS << "#(TableGen'erated file, do not edit!)#\n";
  OS << "#        (Edit ssh.td instead)       #\n";
  OS << "######################################\n";
  OS << "\n\n";
}

static void emitCommon(raw_ostream &OS) {
  OS << "######################################\n";
  OS << "##              Common              ##\n";
  OS << "######################################\n";
  OS << "\n";
}

static void emitHostConfigs(raw_ostream &OS) {
  OS << "######################################\n";
  OS << "##           Host Configs           ##\n";
  OS << "######################################\n";
  OS << "\n";
}

namespace {
class SSHConfigEmitter {
  RecordKeeper &Records;
public:
  explicit SSHConfigEmitter(RecordKeeper &R) : Records(R) {}

  void run(raw_ostream &o);

private:
  void EmitSSHConfig(Record *Cfg, raw_ostream &O, bool PrintHost);
  void EmitAction(Record *Action, unsigned Indent, raw_ostream &O);
  unsigned Counter;
};
} // End anonymous namespace

void SSHConfigEmitter::run(raw_ostream &O) {
  std::vector<Record*> Cfgs = Records.getAllDerivedDefinitions("SSH");

  // Emit common block
  emitCommon(O);
  Record* Common = Records.getDef("Common");
  assert (Common && "No common block defined");
  O << "Host *\n";
  EmitSSHConfig(Common, O, false);
  O << "\n";

  // Emit each host configuration in full.
  emitHostConfigs(O);
  for (unsigned i = 0, e = Cfgs.size(); i != e; ++i) {
    auto name = Cfgs[i]->getName();
    if (name != "Common") {
      EmitSSHConfig(Cfgs[i], O, true);
    }
  }
}

#define EmitConditionalInt(x) do { \
    auto x = Cfg->getValueAsInt(#x);\
    if (x != -1) \
      O << "  " << #x << " " << x << "\n"; \
  } while(0)
#define EmitConditionalString(x) do { \
    auto x = Cfg->getValueAsString(#x);\
    if (x != "") \
      O << "  " << #x << " " << x << "\n"; \
  } while(0)

void SSHConfigEmitter::EmitSSHConfig(Record *Cfg, raw_ostream &O,
                                     bool PrintHost) {
  if (PrintHost) {
    auto aliases = Cfg->getValueAsListOfStrings("Aliases");
    O << "Host " << Cfg->getName();
    for (auto I = aliases.begin(), E = aliases.end(); I != E; ++I)
      O << " " << *I;
    O << "\n";
  }
  EmitConditionalString(User);
  EmitConditionalString(HostName);
  EmitConditionalInt(Port);
  EmitConditionalString(IdentityFile);
  EmitConditionalString(ProxyCommand);
  EmitConditionalString(PreferredAuthentications);
  EmitConditionalString(Compression);
  EmitConditionalInt(ServerAliveInterval);
  O << "\n";
}

namespace llvm {

void EmitSSHConfig(RecordKeeper &RK, raw_ostream &OS) {
  emitSSHHeader(OS);
  SSHConfigEmitter(RK).run(OS);
}

} // End llvm namespace
