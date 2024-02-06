//
// Created by fufeng on 2024/2/5.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum State {
    STATE_INIT,   /* 有限状态自动机的初始状态 */
    STATE_VAR,    /* 接受字符的状态 */
};

enum TokenType {
    TT_VAR,       /* 标示Token类型是变量 */
    TT_INTEGER,   /* token类型是整数 */
    TT_STRING,    /* token类型是字符串 */
};

union TokenValue {
    char *_str;   /* 这里使用了一个union，即可以用于指向字符串，*/
    int _int;   /* 也可以是一个整数值。 */
};

struct Token {
    enum TokenType _type;
    union TokenValue _value;
};

enum State state = STATE_INIT;
char *cur;

struct Token *create_token(enum TokenType tt, char *begin, char *cur) {
    struct Token *nt = (struct Token *) malloc(sizeof(struct Token));
    nt->_type = tt;

    /* 这里只需要对变量进行处理，等号、分号等符号只需要类型就够了。 */
    if (tt == TT_VAR) {
        nt->_value._str = (char *) malloc(cur - begin + 1);
        strncpy(nt->_value._str, begin, cur - begin);
        nt->_value._str[cur - begin] = 0;
    }

    return nt;
}

void destroy_token(struct Token *t) {
    /* 释放空间是和malloc对应的，也在变量的情况下才需要。 */
    if (t->_type == TT_VAR) {
        free(t->_value._str);
        t->_value._str = NULL;
    }

    free(t);
}

struct Token *next_token() {
    state = STATE_INIT;
    char *begin = 0;

    while (*cur) {
        char c = *cur;
        if (state == STATE_INIT) {
            /* 在初态下，遇到空白字符都可以跳过。 */
            if (c == ' ' || c == '\n' || c == '\t') {
                cur++;
                continue;
            }

            /* 遇到字符则认为是一个变量的开始。 */
            if ((c <= 'Z' && c >= 'A') ||
                (c <= 'z' && c >= 'a') || c == '_') {
                begin = cur;
                state = STATE_VAR;
                cur++;
            }
        } else if (state == STATE_VAR) {
            /* 当前状态机处于分析变量的阶段，所以可以继续接受字母和数字。 */
            if ((c <= 'Z' && c >= 'A') ||
                (c <= 'z' && c >= 'a') ||
                (c <= '9' && c >= '0') || c == '_') {
                cur++;
            } else { /* 否则的话，就说明这个变量已经分析完了。 */
                return create_token(TT_VAR, begin, cur);
            }
        }
    }

    return NULL;
}

int main() {
    cur = "int val1 = 1;";

    struct Token *t = next_token();
    printf("%d, %s\n", t->_type, t->_value._str);
    destroy_token(t);

    t = next_token();
    printf("%d, %s\n", t->_type, t->_value._str);
    destroy_token(t);

    return 0;
}