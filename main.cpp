#include <v8.h>
#include <node.h>
#include "LabSound/extended/LabSound.h"
using namespace v8;
void Init(Handle<Object> exports) {
    auto context = lab::MakeRealtimeAudioContext();
}
NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
