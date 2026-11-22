#include <GSgfx.h>
#include <GStexture.h>
#include <libc/memory.h>

// The framebuffer-copy texture is shared by all layers that request the
// shared-copy sentinel.
extern GStexture *gSharedFramebufferCopyTexture;
extern u16 gSharedFramebufferCopyHoldCount;

void GSgfxLayer::GetFifoSize(u32 *pass0Size, u32 *pass1Size)
{
    if (pass0Size != NULL) {
        *pass0Size = mDrawPasses[0].bufferSize;
    }
    if (pass1Size != NULL) {
        *pass1Size = mDrawPasses[1].bufferSize;
    }
}

GStexture *GSgfxLayer::GetTextureCopy(void)
{
    // Copying is only active while the texture-copy flag is set.
    if (!(mRenderFlags & GSGFX_RENDER_FLAG_TEXTURE_COPY)) {
        return NULL;
    }
    return mTextureCopy;
}

void GSgfxLayer::SetTextureCopy(GStexture *texture, bool enableCopy, u8 *copyConfig)
{
    // A NULL texture disables the copy and releases a shared texture hold.
    if (texture == NULL) {
        mRenderFlags &= ~GSGFX_RENDER_FLAG_TEXTURE_COPY;
        if (gSharedFramebufferCopyTexture != NULL && mTextureCopy == gSharedFramebufferCopyTexture) {
            if (--gSharedFramebufferCopyHoldCount == 0) {
                GStextureFree(gSharedFramebufferCopyTexture);
                gSharedFramebufferCopyTexture = NULL;
            }
        }
        mTextureCopy = NULL;
    } else {
        // Several layers may ask for the same framebuffer-copy texture.
        if (texture == GS_GFX_LAYER_SHARED_TEXTURE_COPY) {
            if (++gSharedFramebufferCopyHoldCount == 1) {
                gSharedFramebufferCopyTexture =
                    GStextureCreate(0, 0, GS_TEXTURE_FORMAT_32BIT, GS_TEXTURE_TLUT_FORMAT_NONE, 0);
                if (gSharedFramebufferCopyTexture == NULL) {
                    gSharedFramebufferCopyHoldCount = 0;
                    return;
                }
            }
            texture = gSharedFramebufferCopyTexture;
        }
        mRenderFlags |= GSGFX_RENDER_FLAG_TEXTURE_COPY;
        mTextureCopy = texture;
    }

    if (!enableCopy) {
        mTextureCopyMode = GSGFX_TEXTURE_COPY_DISABLED;
        return;
    }
    if (copyConfig == NULL) {
        mTextureCopyMode = GSGFX_TEXTURE_COPY_NO_CONFIG;
        return;
    }
    mTextureCopyMode = GSGFX_TEXTURE_COPY_CUSTOM_CONFIG;
    memcpy(mTextureCopyConfig, copyConfig, sizeof(mTextureCopyConfig));
}

void GSgfxLayer::SetWantClearFlags(bool color, bool alpha, bool depth)
{
    mRenderFlags &= ~(GSGFX_RENDER_FLAG_WANT_CLEAR_COLOR | GSGFX_RENDER_FLAG_WANT_CLEAR_ALPHA |
                      GSGFX_RENDER_FLAG_WANT_CLEAR_DEPTH);
    if (color) {
        mRenderFlags |= GSGFX_RENDER_FLAG_WANT_CLEAR_COLOR;
    }
    if (alpha) {
        mRenderFlags |= GSGFX_RENDER_FLAG_WANT_CLEAR_ALPHA;
    }
    if (depth) {
        mRenderFlags |= GSGFX_RENDER_FLAG_WANT_CLEAR_DEPTH;
    }
}

void GSgfxLayer::SetClearFlags(bool color, bool alpha, bool depth)
{
    mRenderFlags &= ~(GSGFX_RENDER_FLAG_CLEAR_COLOR | GSGFX_RENDER_FLAG_CLEAR_ALPHA |
                      GSGFX_RENDER_FLAG_CLEAR_DEPTH);
    if (color) {
        mRenderFlags |= GSGFX_RENDER_FLAG_CLEAR_COLOR;
    }
    if (alpha) {
        mRenderFlags |= GSGFX_RENDER_FLAG_CLEAR_ALPHA;
    }
    if (depth) {
        mRenderFlags |= GSGFX_RENDER_FLAG_CLEAR_DEPTH;
    }
}

void GSgfxLayer::SetPassVisibility(bool pass0, bool pass1)
{
    if (pass0 == true) {
        mRenderFlags |= GSGFX_RENDER_FLAG_PASS0_VISIBLE;
    } else {
        mRenderFlags &= ~GSGFX_RENDER_FLAG_PASS0_VISIBLE;
    }
    if (pass1 == true) {
        mRenderFlags |= GSGFX_RENDER_FLAG_PASS1_VISIBLE;
    } else {
        mRenderFlags &= ~GSGFX_RENDER_FLAG_PASS1_VISIBLE;
    }
}

bool GSgfxLayer::IsLayerVisible(void)
{
    return (mRenderFlags & GSGFX_RENDER_FLAG_VISIBLE) != 0;
}

// Visibility is reference-counted so overlapping holders cannot hide a layer.
void GSgfxLayer::SetLayerVisibility(bool on)
{
    if (on == true) {
        mVisibleHolds++;
    } else if (mVisibleHolds != 0) {
        mVisibleHolds--;
    }
}
