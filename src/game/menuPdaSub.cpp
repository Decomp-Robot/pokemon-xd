#include <menuPdaSub.h>
#include <menuSub.h>
#include <time.h>
#include <menuItemBios.h>
#include <GSgfx.h>

// Row alphas of a list: the selected row fades in and the rows around it fade
// out, so a cursor move lights up the new row and dims the old one. Both
// fades follow the frame time, the fade out being the slower of the two.
void menuPdaSubCalcAlpha(f32 *alpha, long count, long selected)
{
    f32 dt = timeGetLastFrameSec();
    f32 step = 255.0f * dt;
    f32 fadeIn = step / 0.1f;
    f32 fadeOut = step / 0.2f;
    long i;

    for (i = 0; i < count; i++) {
        if (selected == i) {
            if (alpha[i] < 255.0f) {
                alpha[i] += fadeIn;
                if (alpha[i] > 255.0f) {
                    alpha[i] = 255.0f;
                }
            }
        } else if (alpha[i] > 0.0f) {
            alpha[i] -= fadeOut;
            if (alpha[i] < 0.0f) {
                alpha[i] = 0.0f;
            }
        }
    }
}

// Puts the row alphas at their resting value: only the selected row is drawn.
void menuPdaSubInitAlpha(f32 *alpha, long count, long selected)
{
    long i;

    for (i = 0; i < count; i++) {
        if (selected == i) {
            alpha[i] = 255.0f;
        } else {
            alpha[i] = 0.0f;
        }
    }
}

// Moves a scroll position toward its target: the step follows the frame time
// and grows while the remaining distance is long. The position snaps to the
// target once the step would pass it.
void menuPdaSubCalcPositon(f32 *pos, f32 target, f32 speed)
{
    f32 rate = speed * timeGetLastFrameSec();
    f32 cur = *pos;
    f32 delta = target - cur;
    f32 step = 30.0f * rate;

    if ((delta > 0.0f ? delta : -delta) >= 100.0f) {
        step *= 30.0f;
    }
    if (delta < 0.0f) {
        step = -step;
    }
    *pos = cur + step;
    delta = target - *pos;
    if ((delta > 0.0f ? delta : -delta) <= (step > 0.0f ? step : -step)) {
        *pos = target;
    }
}

// Scroll bar of a list, drawn as one quad on the layer of the sprite: the
// height of the bar follows the part of the list the window shows. A list
// that fits in the window keeps its bar sprite hidden.
void menuPdaSubScrollBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long menuID, long itemID, u32 color,
                         long count, long visibleCount, GSivec2 *scroll)
{
    enum { SCROLL_BAR_VERTEX_FORMAT_XY_COLOR = 3 };
    GSgfxLayer *layer = GSgfxLayerFindByID(winSpriteGetLayerID(window, sprite));
    tagMENU_ITEM_DATA *item;
    f32 ratio;
    f32 barTop;
    f32 barBottom;

    if (layer == 0) {
        return;
    }
    if (count > visibleCount) {
        sprite->mFlags |= SPRITE_FLAG_DISP;
    } else {
        sprite->mFlags &= ~SPRITE_FLAG_DISP;
        return;
    }
    item = menuItemBiosGetPtr(menuID, itemID);
    ratio = (f32)item->mHeight / (f32)count;
    barTop = ratio * (f32)scroll->x;
    barBottom = ratio * (f32)(scroll->x + visibleCount);
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, true);
    layer->mPrimitiveType = GSGFX_QUADS;
    layer->setRenderStateAndSelect(GSgfxVFGetPredef(SCROLL_BAR_VERTEX_FORMAT_XY_COLOR));
    layer->beginSurface(4);
    GSgfxFifo.s16 = 1;
    GSgfxFifo.s16 = (s16)barTop;
    GSgfxFifo.u32 = color | window->mRgba.mColorA;
    GSgfxFifo.s16 = (s16)(item->mWidth - 2);
    GSgfxFifo.s16 = (s16)barTop;
    GSgfxFifo.u32 = color | window->mRgba.mColorA;
    GSgfxFifo.s16 = (s16)(item->mWidth - 2);
    GSgfxFifo.s16 = (s16)barBottom;
    GSgfxFifo.u32 = color | window->mRgba.mColorA;
    GSgfxFifo.s16 = 1;
    GSgfxFifo.s16 = (s16)barBottom;
    GSgfxFifo.u32 = color | window->mRgba.mColorA;
    layer->endSurface();
}

// Shows or hides the scroll arrow of a list: an arrow is only drawn while the
// cursor can still move in the direction it points at.
void menuPdaSubCheckUpDownCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, CURSOR *cursor, long count,
                                 long visibleCount, bool direction)
{
    sprite->mRgba.mAlpha = menuSubAnimAlpha();
    if (direction) {
        if (count <= visibleCount || cursor->x == 0) {
            winSpriteSetDisp(sprite, 0);
        } else {
            winSpriteSetDisp(sprite, 1);
        }
    } else {
        if (count <= visibleCount || visibleCount == count - cursor->x) {
            winSpriteSetDisp(sprite, 0);
        } else {
            winSpriteSetDisp(sprite, 1);
        }
    }
}
