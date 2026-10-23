#ifndef PDA_MEMO_LIST_H
#define PDA_MEMO_LIST_H

// Pokedex memo list menu. The save-report code closes it through the same
// helper the memo menus use, so only that entry point is declared here; the
// rest of the class is recovered with the memo list unit.
class pdaMemoList {
public:
    void _menuClose(long menu);
};

#endif
