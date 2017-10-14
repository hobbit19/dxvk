#pragma once

#include "dxvk_lifetime.h"

namespace dxvk {
  
  /**
   * \brief DXVK command recorder
   * 
   * An interface that wraps Vulkan calls. \ref DxvkCommandList
   * implements this interface to record Vulkan commands into a
   * primary command buffer, whereas \ref DxvkDeferredCommands
   * buffers the calls and provides methods to record them into
   * a \ref DxvkCommandList on demand.
   */
  class DxvkRecorder : public RcObject {
    
  public:
    
    virtual ~DxvkRecorder();
    
    virtual void beginRecording() = 0;
    virtual void endRecording() = 0;
    
    virtual void cmdBeginRenderPass(
      const VkRenderPassBeginInfo*  pRenderPassBegin,
            VkSubpassContents       contents) = 0;
    
    virtual void cmdBindPipeline(
            VkPipelineBindPoint     pipelineBindPoint,
            VkPipeline              pipeline) = 0;
    
    virtual void cmdClearAttachments(
            uint32_t                attachmentCount,
      const VkClearAttachment*      pAttachments,
            uint32_t                rectCount,
      const VkClearRect*            pRects) = 0;
    
    virtual void cmdDispatch(
            uint32_t                x,
            uint32_t                y,
            uint32_t                z) = 0;
    
    virtual void cmdDraw(
            uint32_t                vertexCount,
            uint32_t                instanceCount,
            uint32_t                firstVertex,
            uint32_t                firstInstance) = 0;
    
    virtual void cmdDrawIndexed(
            uint32_t                indexCount,
            uint32_t                instanceCount,
            uint32_t                firstIndex,
            uint32_t                vertexOffset,
            uint32_t                firstInstance) = 0;
    
    virtual void cmdEndRenderPass();
    
    virtual void trackResource(
      const Rc<DxvkResource>&       rc) = 0;
      
    virtual void reset() = 0;
    
  };
  
}