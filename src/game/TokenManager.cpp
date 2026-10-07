#include "common.h"

extern "C" int strcmp(const char *, const char *);

class TokenManager {
public:
    struct Milestone {
        int threshold;
        int value;
    };
    struct Token {
        char name[0x20];
        int amount;
        int f24;
        Milestone milestones[10];
        int numMilestones;
        int mode;
    };

    Token tokens[32];
    int count;

    TokenManager();
    void init(void);
    int grandTotal(void);
    char *getTokenKey(int i);
    int tokenValue(char *name);
    int tokenValue(int i);
};

INCLUDE_ASM("asm/nonmatchings/game/TokenManager", D_006F1A80);
TokenManager::TokenManager()
{
    init();
}
#ifdef NON_MATCHING
/* 0/38 words, untuned: retail nests two pointer loops */
void TokenManager::init(void)
{
    int i;
    int j;

    count = 0;
    for (i = 0; i < 32; i++) {
        tokens[i].name[0] = 0;
        tokens[i].f24 = 0;
        tokens[i].amount = 0;
        tokens[i].mode = 1;
        for (j = 9; j >= 0; j--)
            tokens[i].milestones[j].threshold = 0;
        tokens[i].numMilestones = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TokenManager", init__12TokenManager);
#endif
INCLUDE_ASM("asm/nonmatchings/game/TokenManager", setMax__12TokenManagerPci);
INCLUDE_ASM("asm/nonmatchings/game/TokenManager", credit__12TokenManagerPci);
int TokenManager::grandTotal(void)
{
    int sum = 0;
    int i;

    for (i = 0; i < count; i++)
        sum += tokenValue(i);
    return sum;
}
char *TokenManager::getTokenKey(int i)
{
    return tokens[i].name;
}
INCLUDE_ASM("asm/nonmatchings/game/TokenManager", setMilestone__12TokenManagerPciiQ212TokenManager13MilestoneType);
#ifdef NON_MATCHING
/* 22/39 words, untuned: loop shape */
int TokenManager::tokenValue(char *name)
{
    int found = 0;
    int i = 0;

    while (i < count) {
        if (strcmp(name, tokens[i].name) == 0) {
            found = 1;
            break;
        }
        i++;
    }
    if (!found)
        return 0;
    return tokenValue(i);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TokenManager", tokenValue__12TokenManagerPc);
#endif
INCLUDE_ASM("asm/nonmatchings/game/TokenManager", tokenValue__12TokenManageri);
