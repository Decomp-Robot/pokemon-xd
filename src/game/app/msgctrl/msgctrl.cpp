#include <types.h>
#include <msgctrl.h>
#include <GSmsg.h>
#include <menuIds.h>
#include <menu.h>
#include <peopleTalk.h>
#include <GSsndPlayback.h>

enum MSGCTRL_RUBY_STATE { MSGCTRL_RUBY_OFF, MSGCTRL_RUBY_BASE, MSGCTRL_RUBY_TOP };
enum { MSGCTRL_FLAG_AUTO_ADVANCE = 2 };

enum MSGCTRL_COLOR_MASK {
    MSGCTRL_COLOR_MASK_ALPHA = 0xFF,
    MSGCTRL_COLOR_MASK_RGB = 0xFFFFFF00,
};

extern "C" {

s32 msgctrlColor(MSG_TASK_WORK *task) {
    if (task->renderPass != 0) {
        u32 alpha = task->color & MSGCTRL_COLOR_MASK_ALPHA;
        task->color = *(u32 *)task->stream;
        task->color &= MSGCTRL_COLOR_MASK_RGB;
        task->color |= alpha;
        GSmsgSetColor(task);
    }
    task->stream += 4;
    return 0;
}

s32 msgctrlFont(MSG_TASK_WORK *task) {
    if (task->renderPass == 0) {
        task->font = *task->stream;
        GSmsgApplyFontMetrics(task);
    } else {
        task->font = *task->stream;
        GSmsgApplyFontMetrics(task);
    }
    task->stream++;
    return 0;
}

s32 msgctrlRubyEnd(MSG_TASK_WORK *task) {
    task->ruby = MSGCTRL_RUBY_OFF;
    return 0;
}

s32 msgctrlRubyTop(MSG_TASK_WORK *task) {
    task->ruby = MSGCTRL_RUBY_TOP;
    return 0;
}

s32 msgctrlRubyStart(MSG_TASK_WORK *task) {
    if (task->renderPass != 0)
        GSmsgCalcRubyLayout(task);
    task->ruby = MSGCTRL_RUBY_BASE;
    return 0;
}

s32 msgctrlKeyWait(MSG_TASK_WORK *task) {
    if ((task->flags & MSGCTRL_FLAG_AUTO_ADVANCE) != 0)
        task->keyWait = 1;
    if (menuIsCheck(MENU_ID_PEOPLE_EDITOR) != 0)
        task->keyWait = 0;
    if (task->renderPass == 0) {
        if (task->keyWait != 0) {
            task->keyWait = 0;
            task->savedStream = task->stream;
            if (*task->stream != 0)
                peopleSetTalkKeyWaitCallback();
            if ((task->flags & MSGCTRL_FLAG_AUTO_ADVANCE) == 0)
                GSsndPlayNow(MENU_SE_CONFIRM);
        } else {
            task->stream -= 3;
            task->keyEndWait = 1;
        }
    } else {
        task->cursorX = task->originX;
        task->cursorY = task->originY;
    }
    return 1;
}

s32 msgctrlCheckWait(MSG_TASK_WORK *task) {
    if ((task->flags & MSGCTRL_FLAG_AUTO_ADVANCE) != 0)
        task->keyWait = 1;
    if (menuIsCheck(MENU_ID_PEOPLE_EDITOR) != 0)
        task->keyWait = 0;
    if (task->renderPass == 0) {
        if (task->keyWait != 0) {
            task->keyWait = 0;
            task->savedStream = task->stream;
            if (*task->stream != 0)
                peopleSetTalkKeyWaitCallback();
            if ((task->flags & MSGCTRL_FLAG_AUTO_ADVANCE) == 0)
                GSsndPlayNow(MENU_SE_CONFIRM);
        } else {
            task->stream -= 3;
            task->checkEndWait = 1;
        }
    } else {
        task->cursorX = task->originX;
        task->cursorY = task->originY;
    }
    return 1;
}

s32 msgctrlCheckEnd(MSG_TASK_WORK *task) {
    if ((task->flags & MSGCTRL_FLAG_AUTO_ADVANCE) != 0)
        task->keyWait = 1;
    if (menuIsCheck(MENU_ID_PEOPLE_EDITOR) != 0)
        task->keyWait = 0;
    if (task->keyWait != 0) {
        task->keyWait = 0;
    } else {
        task->stream -= 3;
        task->checkEndWait = 1;
    }
    return 1;
}

s32 msgctrlKeyEnd(MSG_TASK_WORK *task) {
    if ((task->flags & MSGCTRL_FLAG_AUTO_ADVANCE) != 0)
        task->keyWait = 1;
    if (menuIsCheck(MENU_ID_PEOPLE_EDITOR) != 0)
        task->keyWait = 0;
    if (task->keyWait != 0)
        task->keyWait = 0;
    else
        task->stream -= 3;
    return 1;
}

}
