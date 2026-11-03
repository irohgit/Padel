#pragma once

struct Clip{
    const char* Name;
    int From;
    int To;
    // TODO(animator): Add bool bLoops and priority (maybe) so Hit will play on movement
};
