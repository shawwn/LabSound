// Copyright (c) 2003-2013 Nick Porcino, All rights reserved.
// License is MIT: http://opensource.org/licenses/MIT

#ifndef LabSound_src_RecorderNode_h
#define LabSound_src_RecorderNode_h

#include "AudioBasicInspectorNode.h"
#include "AudioContext.h"
#include <vector>
#include <mutex>

namespace LabSound {
    
    class RecorderNode : public WebCore::AudioBasicInspectorNode {
    public:
        static WTF::PassRefPtr<RecorderNode> create(std::shared_ptr<WebCore::AudioContext> context, float sampleRate)
        {
            return WTF::adoptRef(new RecorderNode(context, sampleRate));
        }
        
        virtual ~RecorderNode();
        
        // AudioNode
        virtual void process(size_t framesToProcess);
        virtual void reset();
        
        void startRecording() { m_recording = true; }
        void stopRecording() { m_recording = false; }
        
        void mixToMono(bool m) { m_mixToMono = m; }

        // replaces result with the currently recorded data.
        // saved data is cleared.
        void getData(std::vector<float>& result);
        
    private:
        virtual double tailTime() const OVERRIDE { return 0; }
        virtual double latencyTime() const OVERRIDE { return 0; }
        
        RecorderNode(std::shared_ptr<WebCore::AudioContext>, float sampleRate);

        bool m_mixToMono;
        bool m_recording;
        std::vector<float> m_data;  // interleaved
        mutable std::recursive_mutex m_mutex;
    };
    
} // LabSound

#endif