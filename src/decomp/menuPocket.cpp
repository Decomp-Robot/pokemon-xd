#include <types.h>

/* degreeRefresh360 -- wrap a degree value into the [0, 360) range. The
 * remainder keeps the sign of the input, so a negative remainder is shifted
 * back up by one full turn. Groups with the other pocket-menu direction
 * helpers (degreeRefresh180, degreeSubHalf, dirRefresh2PI, ...).
 *
 * Note: this translation unit builds with the peephole optimizer disabled.
 * With GC/1.2.5n -O4,p the trailing compare is folded into a recording
 * subtract (subf.), which the original object does not have; GC/1.3.2 with
 * -opt nopeephole reproduces its subf/cmpwi/bgelr sequence exactly. */
long degreeRefresh360(long degree) {
    long result = degree % 360;

    if (result >= 0) {
        return result;
    }

    return result + 360;
}
