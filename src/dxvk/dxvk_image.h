#pragma once

#include "dxvk_format.h"
#include "dxvk_memory.h"
#include "dxvk_resource.h"

namespace dxvk {
  
  /**
   * \brief Image create info
   * 
   * The properties of an image that are
   * passed to \ref DxvkDevice::createImage
   */
  struct DxvkImageCreateInfo {
    /// Image dimension
    VkImageType type;
    
    /// Pixel format
    VkFormat format;
    
    /// Flags
    VkImageCreateFlags flags;
    
    /// Sample count for MSAA
    VkSampleCountFlagBits sampleCount;
    
    /// Image size, in texels
    VkExtent3D extent;
    
    /// Number of image array layers
    uint32_t numLayers;
    
    /// Number of mip levels
    uint32_t mipLevels;
    
    /// Image usage flags
    VkImageUsageFlags usage;
    
    /// Pipeline stages that can access
    /// the contents of the image
    VkPipelineStageFlags stages;
    
    /// Allowed access pattern
    VkAccessFlags access;
    
    /// Image tiling mode
    VkImageTiling tiling;
    
    /// Common image layout
    VkImageLayout layout;
  };
  
  
  /**
   * \brief Image create info
   * 
   * The properties of an image view that are
   * passed to \ref DxvkDevice::createImageView
   */
  struct DxvkImageViewCreateInfo {
    /// Image view dimension
    VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D;
    
    /// Pixel format
    VkFormat format = VK_FORMAT_UNDEFINED;
    
    /// Subresources to use in the view
    VkImageAspectFlags aspect = 0;
    
    uint32_t minLevel  = 0;
    uint32_t numLevels = 0;
    uint32_t minLayer  = 0;
    uint32_t numLayers = 0;
    
    /// Component mapping. Defaults to identity.
    VkComponentMapping swizzle = {
      VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY,
    };
  };
  
  
  /**
   * \brief DXVK image
   * 
   * An image resource consisting of various subresources.
   * Can be accessed by the host if allocated on a suitable
   * memory type and if created with the linear tiling option.
   */
  class DxvkImage : public DxvkResource {
    
  public:
    
    DxvkImage(
      const Rc<vk::DeviceFn>&     vkd,
      const DxvkImageCreateInfo&  createInfo,
            DxvkMemoryAllocator&  memAlloc,
            VkMemoryPropertyFlags memFlags);
    
    /**
     * \brief Creates image object from existing image
     * 
     * This can be used to create an image object for
     * an implementation-managed image. Make sure to
     * provide the correct image properties, since
     * otherwise some image operations may fail.
     */
    DxvkImage(
      const Rc<vk::DeviceFn>&     vkd,
      const DxvkImageCreateInfo&  info,
            VkImage               image);
    
    /**
     * \brief Destroys image
     * 
     * If this is an implementation-managed image,
     * this will not destroy the Vulkan image.
     */
    ~DxvkImage();
    
    /**
     * \brief Image handle
     * 
     * Internal use only.
     * \returns Image handle
     */
    VkImage handle() const {
      return m_image;
    }
    
    /**
     * \brief Image properties
     * 
     * The image create info structure.
     * \returns Image properties
     */
    const DxvkImageCreateInfo& info() const {
      return m_info;
    }
    
    /**
     * \brief Memory type flags
     * 
     * Use this to determine whether a
     * buffer is mapped to host memory.
     * \returns Vulkan memory flags
     */
    VkMemoryPropertyFlags memFlags() const {
      return m_memFlags;
    }
    
    /**
     * \brief Map pointer
     * 
     * If the image has been created on a host-visible
     * memory type, its memory is mapped and can be
     * accessed by the host.
     * \param [in] offset Byte offset into mapped region
     * \returns Pointer to mapped memory region
     */
    void* mapPtr(VkDeviceSize offset) const {
      return m_memory.mapPtr(offset);
    }
    
    /**
     * \brief Image format info
     * \returns Image format info
     */
    const DxvkFormatInfo* formatInfo() const {
      return imageFormatInfo(m_info.format);
    }
    
    /**
     * \brief Size of a mipmap level
     * 
     * \param [in] level Mip level
     * \returns Size of that level
     */
    VkExtent3D mipLevelExtent(uint32_t level) const {
      VkExtent3D size = m_info.extent;
      size.width  = std::max(1u, size.width  >> level);
      size.height = std::max(1u, size.height >> level);
      size.depth  = std::max(1u, size.depth  >> level);
      return size;
    }
    
    /**
     * \brief Queries memory layout of a subresource
     * 
     * Can be used to retrieve the exact pointer to a
     * subresource of a mapped image with linear tiling.
     * \param [in] subresource The image subresource
     * \returns Memory layout of that subresource
     */
    VkSubresourceLayout querySubresourceLayout(
      const VkImageSubresource& subresource) const {
      VkSubresourceLayout result;
      m_vkd->vkGetImageSubresourceLayout(
        m_vkd->device(), m_image,
        &subresource, &result);
      return result;
    }
    
    /**
     * \brief Picks a compatible layout
     * 
     * Under some circumstances, we have to return
     * a different layout than the one requested.
     * \param [in] layout The image layout
     * \returns A compatible image layout
     */
    VkImageLayout pickLayout(VkImageLayout layout) const {
      return m_info.layout == VK_IMAGE_LAYOUT_GENERAL
        ? VK_IMAGE_LAYOUT_GENERAL : layout;
    }
    
  private:
    
    Rc<vk::DeviceFn>      m_vkd;
    DxvkImageCreateInfo   m_info;
    VkMemoryPropertyFlags m_memFlags;
    DxvkMemory            m_memory;
    VkImage               m_image = VK_NULL_HANDLE;
    
  };
  
  
  /**
   * \brief DXVK image view
   */
  class DxvkImageView : public DxvkResource {
    
  public:
    
    DxvkImageView(
      const Rc<vk::DeviceFn>&         vkd,
      const Rc<DxvkImage>&            image,
      const DxvkImageViewCreateInfo&  info);
    
    ~DxvkImageView();
    
    /**
     * \brief Image view handle
     * 
     * Internal use only.
     * \returns Image view handle
     */
    VkImageView handle() const {
      return m_view;
    }
    
    /**
     * \brief Image view type
     * 
     * Convenience method to query the
     * view type in order to check for
     * resource compatibility.
     * \returns Image view type
     */
    VkImageViewType type() const {
      return m_info.type;
    }
    
    /**
     * \brief Image view properties
     * \returns Image view properties
     */
    const DxvkImageViewCreateInfo& info() const {
      return m_info;
    }
    
    /**
     * \brief Image properties
     * \returns Image properties
     */
    const DxvkImageCreateInfo& imageInfo() const {
      return m_image->info();
    }
    
    /**
     * \brief Image format info
     * \returns Image format info
     */
    const DxvkFormatInfo* formatInfo() const {
      return m_image->formatInfo();
    }
    
    /**
     * \brief Image
     * \returns Image
     */
    Rc<DxvkImage> image() const {
      return m_image;
    }
    
    /**
     * \brief Mip level size
     * 
     * Computes the mip level size relative to
     * the first mip level that the view includes.
     * \param [in] level Mip level
     * \returns Size of that level
     */
    VkExtent3D mipLevelExtent(uint32_t level) const {
      return m_image->mipLevelExtent(level + m_info.minLevel);
    }
    
    /**
     * \brief Subresource range
     * \returns Subresource range
     */
    VkImageSubresourceRange subresources() const {
      VkImageSubresourceRange result;
      result.aspectMask     = m_info.aspect;
      result.baseMipLevel   = m_info.minLevel;
      result.levelCount     = m_info.numLevels;
      result.baseArrayLayer = m_info.minLayer;
      result.layerCount     = m_info.numLayers;
      return result;
    }
    
    /**
     * \brief Picks an image layout
     * \see DxvkImage::pickLayout
     */
    VkImageLayout pickLayout(VkImageLayout layout) const {
      return m_image->pickLayout(layout);
    }
    
  private:
    
    Rc<vk::DeviceFn>  m_vkd;
    Rc<DxvkImage>     m_image;
    
    DxvkImageViewCreateInfo m_info;
    VkImageView             m_view;
    
  };
  
}