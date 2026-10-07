#ifndef TOKEN_MANAGER_H
#define TOKEN_MANAGER_H

/* Per-player collectible counters ("tokens"): each has an amount, a maximum and up to 10 milestones. */
class TokenManager {
public:
    enum MilestoneType { MILESTONE_0, MILESTONE_1 };
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
    void setMax(char *name, int max);
    void credit(char *name, int amount);
    void setMilestone(char *name, int threshold, int value, MilestoneType type);
};
typedef char _size_TokenManager[sizeof(TokenManager) == 0x1004 ? 1 : -1];

#endif
