/* ex05_46.c */
/* What does this program do? */
#include <stdio.h>

/* function main begins program execution */
int main( void )
{
   int c; /* variable to hold character input by user */
   
   if ( ( c = getchar() ) != EOF ) {
      main();
      printf( "%c", c );
   }
   
   return 0; /* indicates successful termination */

} /* end main */



/**************************************************************************
 * (C) Copyright 1992-2007 by Deitel & Associates, Inc. and               *
 * Pearson Education, Inc. All Rights Reserved.                           *
 *                                                                        *
 * DISCLAIMER: The authors and publisher of this book have used their     *
 * best efforts in preparing the book. These efforts include the          *
 * development, research, and testing of the theories and programs        *
 * to determine their effectiveness. The authors and publisher make       *
 * no warranty of any kind, expressed or implied, with regard to these    *
 * programs or to the documentation contained in these books. The authors *
 * and publisher shall not be liable in any event for incidental or       *
 * consequential damages in connection with, or arising out of, the       *
 * furnishing, performance, or use of these programs.                     *
 *************************************************************************/
