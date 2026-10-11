#include "common.h"
#include "game/token_manager.h"

extern "C" int strcmp(const char *, const char *);
extern "C" char *strcpy(char *, const char *);
extern "C" int printf(const char *, ...);
extern char tokenOverflowMsg[] __asm__("D_006F1A80");

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
#ifdef NON_MATCHING
/* Sets the cap of token `name`, adding the token if it is new (32 at most). */
void TokenManager::setMax(char *name, int max)
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
    if (i == 32) {
        printf(tokenOverflowMsg, name);
        return;
    }
    tokens[i].f24 = max;
    if (!found) {
        strcpy(tokens[i].name, name);
        count++;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TokenManager", setMax__12TokenManagerPci);
#endif
#ifdef NON_MATCHING
/* Adds `amount` to token `name` (capped at its maximum when it has one), creating the token if it is new. */
void TokenManager::credit(char *name, int amount)
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
    if (i == 32) {
        printf(tokenOverflowMsg, name);
        return;
    }
    {
        int sum = tokens[i].amount + amount;
        int max = tokens[i].f24;

        if (max != 0 && sum >= max)
            sum = max;
        tokens[i].amount = sum;
    }
    if (!found) {
        strcpy(tokens[i].name, name);
        count++;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TokenManager", credit__12TokenManagerPci);
#endif
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
#ifdef NON_MATCHING
/* Score of token `i`: mode 1 = amount (or amount / step * value when the first milestone sets a step); otherwise the value of the highest
   milestone reached that is followed by another one. */
int TokenManager::tokenValue(int i)
{
    Token &t = tokens[i];
    int value = 0;

    if (t.mode == 1) {
        if (t.milestones[0].threshold == 0)
            value = t.amount;
        else
            value = t.amount / t.milestones[0].threshold * t.milestones[0].value;
    } else {
        int n = t.numMilestones - 1;
        int k;

        for (k = 0; k < n; k++) {
            if (t.amount >= t.milestones[k].threshold && t.milestones[k + 1].threshold != 0)
                value = t.milestones[k].value;
        }
    }
    return value;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TokenManager", tokenValue__12TokenManageri);
#endif
