static void OUT__1__8491__(float (*foop__)[10]);

int main(int argc,char **argv)
{
  float foo[10] = {(0.0)};
#pragma omp target data  map(tofrom : foo)
  OUT__1__8491__(&foo);
  return 0;
}

static void OUT__1__8491___COPY(float (*foop__)[10])
{
  ( *foop__)[3] = ((float )4.2);
}

static void OUT__1__8491__(float (*foop__)[10])
{
  ( *foop__)[3] = 4.2;
}
