/*
** main.c
** 
** Started on  Sun Mar 17 06:38:49 2002 mayhem
** Last update Sat Mar 23 11:00:10 2002 mayhem
**
** $Id: main.c,v 1.2 2007-03-07 16:45:36 thor Exp $
**
*/

int	main()
{
  char	*str;
  int	index;

  for (index = 0; index < 10; index++)
    {
      str = (char *) malloc(256);
      memcpy(str, "test_heap", 9);
      printf("%s [%u] \n", str, index);
    }

  puts("End !");
  printf("Hijackme");

  return (0);
}
