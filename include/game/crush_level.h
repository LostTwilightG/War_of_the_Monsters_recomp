#ifndef CRUSH_LEVEL_H
#define CRUSH_LEVEL_H

/* "Crush" mode: collect the most tokens before the timer runs out, sudden death on a tie. */
class CrushLevel {
public:
    void initBeforeDbLoad();
    void initAfterDbLoad();
    void updateNormal();
    void initSuddenDeath();
    void updateSuddenDeath();

    static CrushLevel instance;
    static void (CrushLevel::*UPDATE_FUNK[])();

    int m_timer;       /* 0x0: fields left */
    int m_finished;    /* 0x4 */
    int m_total[2];    /* 0x8: player 1/2 token totals */
    int m_suddenDeath; /* 0x10 */
};

#endif
