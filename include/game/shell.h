#ifndef SHELL_H
#define SHELL_H

/* Front-end shell: level names and file name formatting used by the point tools. */
class Shell {
public:
    char pad0[0x2C10];
    int m_mode; /* 0x2C10: 1 = normal play, 8 = bigshot, 9 = crush */

    char *GetLevelName(void);
    static void formatFilename(char *dst, const char *a, const char *b, const char *c);
};
extern Shell *shell;

#endif
