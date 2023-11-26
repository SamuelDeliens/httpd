#include <criterion/criterion.h>

TestSuite(config);

Test(config, dummy)
{
    cr_assert(1);
}
