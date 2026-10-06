#include "common.h"
#include "game/game.h"
#include "game/MonsterNames.h"

/* "Crush" mode: collect the most tokens before the timer runs out, sudden death on a tie. */
class CrushLevel {
public:
    void initBeforeDbLoad();
    void initAfterDbLoad();
    void updateNormal();
    void initSuddenDeath();
    void updateSuddenDeath();

    int m_timer;       /* 0x0: fields left */
    int m_finished;    /* 0x4 */
    int m_total[2];    /* 0x8: player 1/2 token totals */
    int m_suddenDeath; /* 0x10 */
};

void CrushLevel::initBeforeDbLoad()
{
}

void CrushLevel::initAfterDbLoad()
{
    int i;

    m_finished = 0;
    m_timer = 45 * 60;
    m_suddenDeath = 0;
    Cameras::InitCrushMonsters(game->m_monsters[0], game->m_monsters[1]);
    gHudEnable = 0;
    for (i = 0; i < 2; i++)
        game->m_monsters[i]->drainSpecial();
}

void CrushLevel::updateNormal()
{
    char buf[32];
    Monster *loser;
    Monster *winner;

    m_timer -= timerGetFieldsLastFrame();
    if (m_timer > 0) {
        fontSetColor(0, 0x5A, 0x5A, 0x5A, 0x80);
        fontSetCharSizesInSubPixels(0, 0x280, 0x140, -0x11, -21);
        sprintf(buf, "%d", m_timer / 60);
        fontSpritePrintCenteredXY(0, 0x140, 0xE, buf);
        fontSetColor(0, 0xFF, 0, 0, 0x80);
        sprintf(buf, "P1 %d", game->m_tokens[0].grandTotal());
        fontSpritePrintXY(0, 0x20, 0xC8, buf);
        fontSetColor(0, 0, 0, 0xFF, 0x80);
        sprintf(buf, "P2 %d", game->m_tokens[1].grandTotal());
        fontSpritePrintRightXY(0, 0x271, 0xC8, buf);
        fontSetDefaultColor(0);
        fontSetDefaultSize(0);
        return;
    }

    if (m_finished == 0) {
        m_total[0] = game->m_tokens[0].grandTotal();
        m_total[1] = game->m_tokens[1].grandTotal();
        if (m_total[0] == m_total[1]) {
            initSuddenDeath();
            return;
        }
        if (m_total[0] > m_total[1]) {
            game->m_won[0] = 0;
            game->m_won[1] = 1;
            winner = game->m_monsters[0];
            loser = game->m_monsters[1];
        } else {
            game->m_won[1] = 0;
            game->m_won[0] = 1;
            winner = game->m_monsters[1];
            loser = game->m_monsters[0];
        }
        loser->m_unkF9 = 0;
        winner->m_unkF7 = 0;
        winner->enterNewState((MonsterState *)winner->m_victoryState);
        m_finished = 1;
    }
    fontSetColor(0, 0x5A, 0x5A, 0x5A, 0x80);
    fontSetCharSizesInSubPixels(0, 0x280, 0x140, -0x11, -21);
    sprintf(buf, "%s WINS\n", MonsterNewNames[game->m_monsters[game->m_won[1]]->m_typeBits >> 5]);
    fontSpritePrintCenteredXY(0, 0x140, 0xE, buf);
    fontSetDefaultSize(0);
    fontSetDefaultColor(0);
}

void CrushLevel::initSuddenDeath()
{
    int i;

    m_suddenDeath = 1;
    for (i = 0; i < 2; i++) {
        game->m_monsters[i]->m_unkEC = 1;
        game->m_monsters[i]->drainSpecial();
    }
}

void CrushLevel::updateSuddenDeath()
{
    char buf[16];

    fontSetColor(0, 0x5A, 0x5A, 0x5A, 0x80);
    fontSetCharSizesInSubPixels(0, 0x280, 0x140, -0x11, -21);
    sprintf(buf, "SUDDEN DEATH");
    fontSpritePrintCenteredXY(0, 0x140, 0xE, buf);
    fontSetDefaultColor(0);
    fontSetDefaultSize(0);
}
