/* ========================================================================
   LISTING 1
   ======================================================================== */

int __declspec(noinline) add(int A, int B)
{
    return A + B;
}

#pragma optimize("", off)
int main(int ArgCount, char **Args)
{
    return add(1234, 5678);
}