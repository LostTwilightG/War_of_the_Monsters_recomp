#ifndef SHELL_H
#define SHELL_H

struct _hierhead;
enum _vramAddrs { VRAM_ADDRS_DUMMY };

/* Front-end shell: level names and file name formatting used by the point tools. */
class Shell {
public:
    char pad0[0x2938];
    int m_monsterSel[8];  /* 0x2938: per slot (players first, then AIs): (monster << 5) | variant; monster indexes MonsterLongNames */
    char pad2958[0x2BB0 - 0x2958];
    int m_levelNum;       /* 0x2BB0: index into GameLevelNames (0 = the level named on the command line) */
    int m_numPlayers;     /* 0x2BB4 */
    int m_numAIs;         /* 0x2BB8 */
    char pad2BBC[0x2BE0 - 0x2BBC];
    int m_costume[8];     /* 0x2BE0: costume number of the first two players, then of the AIs */
    char pad2C00[0x2C10 - 0x2C00];
    int m_mode; /* 0x2C10: 1 = normal play, 8 = bigshot, 9 = crush */

    enum _shFileType { SH_FILE_0, SH_FILE_PLAYER, SH_FILE_AI };

    char *GetLevelName(void);
    void LoadLevelFiles(void);
    void LoadLevelDB(void);
    void LoadMonstersDB(void);
    void LoadResTexture(void);
    void LoadTexture(void);
    _vramAddrs getVramAddr(void);
    void AddEpNode(int i, _hierhead *h);
    void formatFilename1(char *dst, const char *name, int costume, const char *ext, _shFileType t);
    void formatFilename(char *dst, const char *level, const char *ext, _shFileType t);
    static void formatFilename(char *dst, const char *a, const char *b, const char *c);
};
extern Shell *shell;

#endif
