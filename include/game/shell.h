#ifndef SHELL_H
#define SHELL_H

/* Front-end shell: level names and file name formatting used by the point tools. */
class Shell {
public:
    char *GetLevelName(void);
    static void formatFilename(char *dst, const char *a, const char *b, const char *c);
};
extern Shell *shell;

#endif
