#ifndef DEBUG_H
#define DEBUG_H

#include <cstdio>
#include <exception>

#define DVX(str) { std::printf str ; std::printf("\n"); std::fflush(stdout); }

#ifdef HG_DEBUG
#define DV(str) { std::printf str ; std::printf("\n"); std::fflush(stdout); }
#else
#define DV(str) { }
#endif

/*!
 * Throws exception to exit the program.
 */
class ProgError
{
    public:
    ProgError(const QString& reason)
    {
        DVX(("ProgError: %s", qPrintable(reason)));
        throw std::exception(qPrintable(reason));
    }
};

#define HG_ASSERT(condition, reason) { if (!condition) ProgError(reason); }

#endif
