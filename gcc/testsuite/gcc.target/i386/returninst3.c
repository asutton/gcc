/* { dg-do compile } */
/* { dg-options "-pg -mfentry -minstrument-return=call" } */
/* { dg-final { scan-assembler-not "call.*__return__" } } */

__attribute__((no_instrument_function))
int func(int a)
{
  return a+1;
}