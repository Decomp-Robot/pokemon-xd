#include <math.h>
#include <dolphin/os.h>
#include <hsd/assert.h>
#include <hsd/bytecode.h>
#include <hsd/debug.h>
#include <hsd/list.h>
#include <hsdRandom.h>

/* Ops that work on angles take and return degrees. */
#define HSD_BYTECODE_PI 3.14159265358979323846
static inline f32 HSD_ByteCodeDegToRad(f32 deg)
{
    return (HSD_BYTECODE_PI / 180.0) * deg;
}

static inline f32 HSD_ByteCodeRadToDeg(f32 rad)
{
    return (180.0 / HSD_BYTECODE_PI) * rad;
}

#define HSD_BYTECODE_HALF_PI ((f32) (HSD_BYTECODE_PI / 2.0))

typedef enum HSD_ByteCodeOpcode {
    HSD_BYTECODE_NOP = 0,
    HSD_BYTECODE_RETURN = 1,
    HSD_BYTECODE_PUSH_ARG = 2,
    HSD_BYTECODE_JUMP_IF = 3,
    HSD_BYTECODE_JUMP = 4,
    HSD_BYTECODE_POP = 5,
    HSD_BYTECODE_PUSH_IMM = 6,
    HSD_BYTECODE_TO_INT = 7,
    HSD_BYTECODE_TO_FLOAT = 8,
    HSD_BYTECODE_NEG_FLOAT = 9,
    HSD_BYTECODE_NEG_INT = 10,
    HSD_BYTECODE_RAND_INT = 11,
    HSD_BYTECODE_RAND_FLOAT = 12,
    HSD_BYTECODE_SIN = 13,
    HSD_BYTECODE_COS = 14,
    HSD_BYTECODE_TAN = 15,
    HSD_BYTECODE_ASIN = 16,
    HSD_BYTECODE_ACOS = 17,
    HSD_BYTECODE_ATAN = 18,
    HSD_BYTECODE_LOG = 19,
    HSD_BYTECODE_EXP = 20,
    HSD_BYTECODE_ABS_FLOAT = 21,
    HSD_BYTECODE_SQRT = 22,
    HSD_BYTECODE_ADD_FLOAT = 23,
    HSD_BYTECODE_SUB_FLOAT = 24,
    HSD_BYTECODE_MUL_FLOAT = 25,
    HSD_BYTECODE_DIV_FLOAT = 26,
    HSD_BYTECODE_MOD_FLOAT = 27,
    HSD_BYTECODE_ADD_INT = 28,
    HSD_BYTECODE_SUB_INT = 29,
    HSD_BYTECODE_MUL_INT = 30,
    HSD_BYTECODE_DIV_INT = 31,
    HSD_BYTECODE_MOD_INT = 32,
    HSD_BYTECODE_POW = 33,
    HSD_BYTECODE_MIN_FLOAT = 34,
    HSD_BYTECODE_MAX_FLOAT = 35,
    HSD_BYTECODE_MIN_INT = 36,
    HSD_BYTECODE_MAX_INT = 37,
    HSD_BYTECODE_ATAN2 = 38,
    HSD_BYTECODE_RAND_RANGE = 39,
    HSD_BYTECODE_ABS_INT = 40,
    HSD_BYTECODE_LESS_INT = 41,
    HSD_BYTECODE_GREATER_INT = 42,
    HSD_BYTECODE_LESS_EQUAL_INT = 43,
    HSD_BYTECODE_GREATER_EQUAL_INT = 44,
    HSD_BYTECODE_EQUAL_INT = 45,
    HSD_BYTECODE_NOT_EQUAL_INT = 46,
    HSD_BYTECODE_LOGICAL_AND = 47,
    HSD_BYTECODE_LOGICAL_OR = 48,
    HSD_BYTECODE_LOGICAL_NOT = 49,
    HSD_BYTECODE_LOGICAL_XOR = 50,
    HSD_BYTECODE_LESS_FLOAT = 51,
    HSD_BYTECODE_GREATER_FLOAT = 52,
    HSD_BYTECODE_LESS_EQUAL_FLOAT = 53,
    HSD_BYTECODE_GREATER_EQUAL_FLOAT = 54,
    HSD_BYTECODE_EQUAL_FLOAT = 55,
    HSD_BYTECODE_NOT_EQUAL_FLOAT = 56,
    HSD_BYTECODE_AND_INT = 57,
    HSD_BYTECODE_OR_INT = 58,
    HSD_BYTECODE_XOR_INT = 59,
    HSD_BYTECODE_PICK = 60,
    HSD_BYTECODE_UNIMPLEMENTED = 255
} HSD_ByteCodeOpcode;

/* A stack slot carries a float or an integer in the list data word. */
typedef union {
    f32 f;
    s32 i;
} HSD_ByteCodeValue;

#define HSD_BYTECODE_TOP(list) (*(HSD_ByteCodeValue *) &(list)->data)
#define HSD_BYTECODE_FLOAT_DATA(value) (*(void **) &(value))
#define HSD_BYTECODE_ASSERT_BINARY(line, list) \
    do { \
        HSD_ASSERTLINE(line, list); \
        HSD_ASSERTLINE(line, list->next); \
    } while (0)

f32 HSD_ByteCodeEval(u8 *bytecode, f32 *args, s32 nb_args)
{
    HSD_SList *stack = NULL;
    f32 value;
    f32 arg;
    s32 rhs;
    s32 lower;
    s32 operand_bytes = 0;
    u8 op;
    u32 operand;
    if (bytecode == NULL) {
        return 0.0F;
    }

    while (1) {
        /* The operand bytes of the previous instruction are collected here. */
        if (operand_bytes > 0) {
            operand = operand << 8 | *bytecode++;
            if (--operand_bytes) {
                continue;
            }

            switch (op) {
            case HSD_BYTECODE_PUSH_ARG:
                HSD_ASSERTLINE(281, operand < nb_args);
                stack = HSD_SListPrepend(stack, HSD_BYTECODE_FLOAT_DATA(args[operand]));
                continue;

            case HSD_BYTECODE_POP:
                {
                    u32 index;
                    for (index = 0; index < operand; ++index) {
                        stack = HSD_SListRemove(stack);
                    }
                }
                continue;

            case HSD_BYTECODE_PICK:
                {
                    HSD_SList *node = stack;

                    lower = 0;
                    while (node != NULL && lower < operand) {
                        node = node->next;
                        lower++;
                    }

                    if (node == NULL) {
                        OSReport("specified stack doesn't exist (%d).\n", operand);
                        HSD_Panic(__FILE__, 299, "");
                        continue;
                    }

                    stack = HSD_SListPrepend(stack, node->data);
                }
                continue;

            case HSD_BYTECODE_JUMP_IF:
                HSD_ASSERTLINE(307, stack);
                if (HSD_BYTECODE_TOP(stack).i != 0) {
                    bytecode += operand;
                }
                stack = HSD_SListRemove(stack);
                continue;

            case HSD_BYTECODE_JUMP:
                bytecode += operand;
                continue;

            case HSD_BYTECODE_PUSH_IMM:
                stack = HSD_SListPrepend(stack, (void *) operand);
                continue;

            case HSD_BYTECODE_UNIMPLEMENTED:
                HSD_Panic(__FILE__, 323, "not yet implemented.\n");

            default:
                HSD_Panic(__FILE__, 326, "unexpected byte code.\n");
                continue;
            }
        }

        op = *bytecode++;

        switch (op) {
        case HSD_BYTECODE_NOP:
            break;

        case HSD_BYTECODE_RETURN:
            HSD_ASSERTLINE(339, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            while (stack != NULL) {
                stack = HSD_SListRemove(stack);
            }
            return arg;

        case HSD_BYTECODE_POP:
        case HSD_BYTECODE_PICK:
        case HSD_BYTECODE_UNIMPLEMENTED:
            operand_bytes = 1;
            operand = 0;
            break;

        case HSD_BYTECODE_PUSH_ARG:
        case HSD_BYTECODE_JUMP_IF:
        case HSD_BYTECODE_JUMP:
            operand_bytes = 2;
            operand = 0;
            break;

        case HSD_BYTECODE_PUSH_IMM:
            operand_bytes = 4;
            operand = 0;
            break;

        case HSD_BYTECODE_TO_INT:
            HSD_ASSERTLINE(376, stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).f;
            break;

        case HSD_BYTECODE_TO_FLOAT:
            HSD_ASSERTLINE(381, stack);
            value = HSD_BYTECODE_TOP(stack).i;
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_NEG_FLOAT:
            HSD_ASSERTLINE(387, stack);
            value = -HSD_BYTECODE_TOP(stack).f;
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_NEG_INT:
            HSD_ASSERTLINE(393, stack);
            HSD_BYTECODE_TOP(stack).i = -HSD_BYTECODE_TOP(stack).i;
            break;

        case HSD_BYTECODE_RAND_INT:
            HSD_ASSERTLINE(399, stack);
            HSD_BYTECODE_TOP(stack).i = HSD_Randi(2);
            break;

        case HSD_BYTECODE_RAND_FLOAT:
            HSD_ASSERTLINE(405, stack);
            value = HSD_Randf();
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_SIN:
            HSD_ASSERTLINE(411, stack);
            value = sin(HSD_ByteCodeDegToRad(HSD_BYTECODE_TOP(stack).f));
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_COS:
            HSD_ASSERTLINE(417, stack);
            value = cos(HSD_ByteCodeDegToRad(HSD_BYTECODE_TOP(stack).f));
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_TAN:
            HSD_ASSERTLINE(423, stack);
            value = tan(HSD_ByteCodeDegToRad(HSD_BYTECODE_TOP(stack).f));
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_ASIN:
            HSD_ASSERTLINE(429, stack);
            value = HSD_ByteCodeRadToDeg(asin(HSD_BYTECODE_TOP(stack).f));
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_ACOS:
            HSD_ASSERTLINE(435, stack);
            value = HSD_ByteCodeRadToDeg(acos(HSD_BYTECODE_TOP(stack).f));
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_ATAN:
            HSD_ASSERTLINE(441, stack);
            value = HSD_ByteCodeRadToDeg(atan(HSD_BYTECODE_TOP(stack).f));
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_LOG:
            HSD_ASSERTLINE(447, stack);
            value = log(HSD_BYTECODE_TOP(stack).f);
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_EXP:
            HSD_ASSERTLINE(453, stack);
            value = exp(HSD_BYTECODE_TOP(stack).f);
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_ABS_FLOAT:
            HSD_ASSERTLINE(459, stack);
            if (HSD_BYTECODE_TOP(stack).f < 0.0F) {
                value = -HSD_BYTECODE_TOP(stack).f;
                stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            }
            break;

        case HSD_BYTECODE_ABS_INT:
            HSD_ASSERTLINE(467, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            if (rhs < 0) {
                HSD_BYTECODE_TOP(stack).i = -rhs;
            }
            break;

        case HSD_BYTECODE_SQRT:
            HSD_ASSERTLINE(474, stack);
            value = sqrtf(HSD_BYTECODE_TOP(stack).f);
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_LOGICAL_NOT:
            HSD_ASSERTLINE(480, stack);
            HSD_BYTECODE_TOP(stack).i = !HSD_BYTECODE_TOP(stack).i;
            break;

        case HSD_BYTECODE_ADD_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(501, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            value = HSD_BYTECODE_TOP(stack).f + arg;
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_SUB_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(507, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            value = HSD_BYTECODE_TOP(stack).f - arg;
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_MUL_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(513, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            value = HSD_BYTECODE_TOP(stack).f * arg;
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_DIV_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(519, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            value = HSD_BYTECODE_TOP(stack).f / arg;
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_MOD_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(525, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            value = fmod(HSD_BYTECODE_TOP(stack).f, arg);
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_ADD_INT:
            HSD_BYTECODE_ASSERT_BINARY(531, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i + rhs;
            break;

        case HSD_BYTECODE_SUB_INT:
            HSD_BYTECODE_ASSERT_BINARY(536, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i - rhs;
            break;

        case HSD_BYTECODE_MUL_INT:
            HSD_BYTECODE_ASSERT_BINARY(541, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i * rhs;
            break;

        case HSD_BYTECODE_DIV_INT:
            HSD_BYTECODE_ASSERT_BINARY(546, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i / rhs;
            break;

        case HSD_BYTECODE_MOD_INT:
            HSD_BYTECODE_ASSERT_BINARY(551, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i % rhs;
            break;

        case HSD_BYTECODE_POW:
            HSD_BYTECODE_ASSERT_BINARY(556, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            value = pow(HSD_BYTECODE_TOP(stack).f, arg);
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_MIN_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(562, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            if (HSD_BYTECODE_TOP(stack).f > arg) {
                stack->data = HSD_BYTECODE_FLOAT_DATA(arg);
            }
            break;

        case HSD_BYTECODE_MAX_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(569, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            if (HSD_BYTECODE_TOP(stack).f < arg) {
                stack->data = HSD_BYTECODE_FLOAT_DATA(arg);
            }
            break;

        case HSD_BYTECODE_MIN_INT:
            HSD_BYTECODE_ASSERT_BINARY(576, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            if (HSD_BYTECODE_TOP(stack).i > rhs) {
                HSD_BYTECODE_TOP(stack).i = rhs;
            }
            break;

        case HSD_BYTECODE_MAX_INT:
            HSD_BYTECODE_ASSERT_BINARY(583, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            if (HSD_BYTECODE_TOP(stack).i < rhs) {
                HSD_BYTECODE_TOP(stack).i = rhs;
            }
            break;

        case HSD_BYTECODE_ATAN2:
            HSD_BYTECODE_ASSERT_BINARY(590, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            {
                f32 y = HSD_BYTECODE_TOP(stack).f;
                f32 x = arg;

                value = HSD_ByteCodeRadToDeg(x == 0.0F
                                                 ? (y >= 0.0F ? HSD_BYTECODE_HALF_PI
                                                              : -HSD_BYTECODE_HALF_PI)
                                                 : (f32) atan2(y, x));
            }
            stack->data = HSD_BYTECODE_FLOAT_DATA(value);
            break;

        case HSD_BYTECODE_LESS_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(596, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).f < arg;
            break;

        case HSD_BYTECODE_GREATER_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(601, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).f > arg;
            break;

        case HSD_BYTECODE_LESS_EQUAL_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(606, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).f <= arg;
            break;

        case HSD_BYTECODE_GREATER_EQUAL_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(611, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).f >= arg;
            break;

        case HSD_BYTECODE_EQUAL_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(616, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).f == arg;
            break;

        case HSD_BYTECODE_NOT_EQUAL_FLOAT:
            HSD_BYTECODE_ASSERT_BINARY(621, stack);
            arg = HSD_BYTECODE_TOP(stack).f;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).f != arg;
            break;

        case HSD_BYTECODE_LESS_INT:
            HSD_BYTECODE_ASSERT_BINARY(626, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i < rhs;
            break;

        case HSD_BYTECODE_GREATER_INT:
            HSD_BYTECODE_ASSERT_BINARY(631, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i > rhs;
            break;

        case HSD_BYTECODE_LESS_EQUAL_INT:
            HSD_BYTECODE_ASSERT_BINARY(636, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i <= rhs;
            break;

        case HSD_BYTECODE_GREATER_EQUAL_INT:
            HSD_BYTECODE_ASSERT_BINARY(641, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i >= rhs;
            break;

        case HSD_BYTECODE_EQUAL_INT:
            HSD_BYTECODE_ASSERT_BINARY(646, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i == rhs;
            break;

        case HSD_BYTECODE_NOT_EQUAL_INT:
            HSD_BYTECODE_ASSERT_BINARY(651, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i != rhs;
            break;

        case HSD_BYTECODE_LOGICAL_AND:
            HSD_BYTECODE_ASSERT_BINARY(656, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i && rhs;
            break;

        case HSD_BYTECODE_LOGICAL_OR:
            HSD_BYTECODE_ASSERT_BINARY(661, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i || rhs;
            break;

        case HSD_BYTECODE_LOGICAL_XOR:
            HSD_BYTECODE_ASSERT_BINARY(666, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            {
                s32 lhs = HSD_BYTECODE_TOP(stack).i;
                HSD_BYTECODE_TOP(stack).i =
                    (lhs == 0 && rhs != 0) || (lhs != 0 && rhs == 0);
            }
            break;

        case HSD_BYTECODE_AND_INT:
            HSD_BYTECODE_ASSERT_BINARY(671, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i & rhs;
            break;

        case HSD_BYTECODE_OR_INT:
            HSD_BYTECODE_ASSERT_BINARY(676, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i | rhs;
            break;

        case HSD_BYTECODE_XOR_INT:
            HSD_BYTECODE_ASSERT_BINARY(681, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            HSD_BYTECODE_TOP(stack).i = HSD_BYTECODE_TOP(stack).i ^ rhs;
            break;

        case HSD_BYTECODE_RAND_RANGE:
            HSD_BYTECODE_ASSERT_BINARY(687, stack);
            rhs = HSD_BYTECODE_TOP(stack).i;
            stack = HSD_SListRemove(stack);
            lower = HSD_BYTECODE_TOP(stack).i;
            HSD_BYTECODE_TOP(stack).i = lower + HSD_Randi(rhs - lower + 1);
            break;

        default:
            OSReport("unexpected opcode 0x%x.\n", op);
            HSD_Panic(__FILE__, 693, "");
            break;
        }
    }
}
