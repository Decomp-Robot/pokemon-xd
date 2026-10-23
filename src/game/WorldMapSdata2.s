.include "macros.inc"
.file "WorldMapSdata2.s"

# Alignment staging for the world map module's sdata2 pool. The original
# build's sdata2 range for WorldMap.cpp holds only the 8-byte int-to-float
# bias double that its long->float conversions share (worldMapIntToFloatBias,
# 0x804EBF08); the four zero bytes in front of it are the padding that aligns
# that double. This object owns the padding bytes and carries the symbol
# table's name for the bias so that units still linked from their extracted
# objects (WorldMapIcon.o's WorldMap::GetAngle) resolve it; the bias data
# itself is compiler-generated pool data emitted by WorldMap.cpp.

.section .sdata2, "a"
.balign 4
.4byte 0
.sym worldMapIntToFloatBias, global
.endsym worldMapIntToFloatBias
