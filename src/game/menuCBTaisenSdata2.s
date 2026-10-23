.include "macros.inc"
.file "menuCBTaisenSdata2.s"

# Constants the entry menus' access flash blinks with: the length of one blink
# cycle, the unity its mirrored phase is folded down from, the gain that turns
# the folded phase into the flash strength, the period the accumulated phase
# wraps at, and the strength to pixel size scale of the marks.

.section .sdata2, "a"
.balign 4

# Length of one blink cycle: the phase is normalised with it, folds over at it
# and wraps at it; the entry menu arms the screen fade at the same level.
.sym menuCBAccessFlashCyclePeriod, global
.4byte 0x3F000000 # 0.5f
.endsym menuCBAccessFlashCyclePeriod

# Unity the mirrored phase is folded down from.
.sym menuCBAccessFlashUnity, global
.4byte 0x3F800000 # 1.0f
.endsym menuCBAccessFlashUnity

# Gain that turns the folded phase into the flash strength.
.sym menuCBAccessFlashGain, global
.4byte 0x40000000 # 2.0f
.endsym menuCBAccessFlashGain

# Period the accumulated phase wraps at (the argument of fmod).
.balign 8
.sym menuCBAccessFlashWrapPeriod, global
.8byte 0x3FE0000000000000 # 0.5
.endsym menuCBAccessFlashWrapPeriod

# Strength to pixel size scale of the blinking marks.
.balign 4
.sym menuCBAccessFlashSizeScale, global
.4byte 0x40A00000 # 5.0f
.endsym menuCBAccessFlashSizeScale
