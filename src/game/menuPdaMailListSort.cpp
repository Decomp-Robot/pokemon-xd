#include <GSmem.h>
#include <menu.h>
#include <menuPdaMailListSort.h>
#include <types.h>

void _pdaMailSort(void);

extern u16 *_sortwork;

extern "C" s32 mailGetNbMailInMailbox(void);

// Closes the given menu, waiting for the close to finish.
void _menuClose(long menuId)
{
    menuClose(menuId);
    menuCloseSync(menuId, 1);
}

// Releases the mail list sort buffer allocated by menuPdaMailListSortInit.
void menuPdaMailListSortExit(void)
{
    if (_sortwork != 0 && _sortwork != 0) {
        GSmemFree(_sortwork);
        _sortwork = 0;
    }
}

// Allocates the sort buffer for the mailbox, sorts it and returns the
// index of the given mail id, or -1 if there is no such mail.
s32 menuPdaMailListSortInit(s32 mailId)
{
    s32 count = mailGetNbMailInMailbox();
    u16 *buffer = (count > 0) ? (u16 *)GSmemAlloc(count * 2) : 0;

    _sortwork = buffer;
    if (buffer != 0) {
        _pdaMailSort();
        buffer = _sortwork;
        for (s32 i = 0; i < count; buffer++, i++) {
            if (mailId == *buffer) {
                return i;
            }
        }
        return -1;
    }
    return -1;
}
