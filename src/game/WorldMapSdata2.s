.include "macros.inc"
.file "WorldMapSdata2.s"

# .sdata2 data of the WorldMap module: the four bytes ahead of the module's
# int-to-float bias, and the shared name of that bias entry.
.section .sdata2, "a"
.balign 4

.4byte 0x00000000

.sym lbl_804EBF08, global
.endsym lbl_804EBF08
