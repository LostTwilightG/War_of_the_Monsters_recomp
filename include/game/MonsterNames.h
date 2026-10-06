#ifndef MONSTERNAMES_H
#define MONSTERNAMES_H

/* Internal monster names, indexed by monster type. Defined static in a header: each TU that uses it
 * gets its own copy (5 copies in the retail binary). */
static const char MonsterNewNames[17][12] = {
    "null",  "congar", "robo-47",  "togera", "ultra-v", "preytor", "spider", "kineticlops",
    "magmo", "raptros", "agamo", "zorgulon", "ogre",    "assboss", "jelly",  "final",
    "temp16",
};

#endif
