/*
 * fsysTOC2.c
 *
 * The catalog side of the file system: the gsfsys.toc catalog that maps
 * group ids to the groups of the disc image, and the per-group expand
 * tracking the daemons poll while their reads run.
 */

#include <dolphin/os.h>

#include <GSAPI/GSfsys/fsys.h>

#pragma force_active on
char const fsysTOCFormat[] = "%d\n";
#pragma force_active reset
char const fsysTOCFilename[] = "gsfsys.toc";

/*! Load the gsfsys.toc catalog off the disc into memory of its own and rebase
 *  the group paths it keeps onto the loaded image. */
u8 _fsysInitTOC(void)
{
    u32 length;
    GSdvdFile* file;
    FsysGroup* group;
    u32 index;

    file = GSdvdOpen(fsysTOCFilename);
    if (file == NULL) {
        return 0;
    }
    length = (GSdvdGetLength(file) + 31) & ~31;
    _fsysTOC = _fsysDoMalloc(length);
    if (_fsysTOC == NULL) {
        return 0;
    }
    if (GSdvdRead(file, _fsysTOC, length, 0) < 0) {
        GSdvdClose(file);
        return 0;
    }
    GSdvdClose(file);

    group = (FsysGroup*)((u8*)_fsysTOC + _fsysTOC->groupTableOffset);
    for (index = 0; index < _fsysTOC->groupCount; index++) {
        group->path += (u32)_fsysTOC;
        group->expandFlags = 0;
        group++;
    }
    return 1;
}
