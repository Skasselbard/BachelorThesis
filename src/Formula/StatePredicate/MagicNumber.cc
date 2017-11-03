#include <Formula/StatePredicate/MagicNumber.h>

int64_t MagicNumber::nextMagicNumber = MAGIC_NUMBER_INITIAL;

int64_t MagicNumber::assign()
{
	return nextMagicNumber++;
}
