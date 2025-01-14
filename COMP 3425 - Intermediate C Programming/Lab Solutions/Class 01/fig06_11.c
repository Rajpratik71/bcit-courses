// Fig. 6.11: fig06_11.cpp
// Craps simulation.

/*
#include <iostream>
using std::cout;
using std::endl;

#include <cstdlib> // contains prototypes for functions srand and rand
using std::rand;
using std::srand;

#include <ctime> // contains prototype for function time
using std::time;
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int rollDice(); // rolls dice, calculates amd displays sum

int main()
{
   // enumeration with constants that represent the game status
   enum Status { CONTINUE, WON, LOST };

   int myPoint; // point if no win or loss on first roll
   enum Status gameStatus; // can contain CONTINUE, WON or LOST
   int sumOfDice; // PLEASE NOTE - declaration must come before executables
                  //   when programming in C

   // randomize random number generator using current time
   srand( time( 0 ) ); 

   sumOfDice = rollDice(); // first roll of the dice

   // determine game status and point (if needed) based on first roll
   switch ( sumOfDice ) 
   {
      case 7: // win with 7 on first roll
      case 11: // win with 11 on first roll           
         gameStatus = WON;
         break;
      case 2: // lose with 2 on first roll
      case 3: // lose with 3 on first roll
      case 12: // lose with 12 on first roll             
         gameStatus = LOST;
         break;
      default: // did not win or lose, so remember point
         gameStatus = CONTINUE; // game is not over
         myPoint = sumOfDice; // remember the point
         printf("\nPoint is %d\n", myPoint);
         break; // optional at end of switch  
   } // end switch 

   // while game is not complete
   while ( gameStatus == CONTINUE ) // not WON or LOST
   { 
      sumOfDice = rollDice(); // roll dice again

      // determine game status
      if ( sumOfDice == myPoint ) // win by making point
         gameStatus = WON;
      else
         if ( sumOfDice == 7 ) // lose by rolling 7 before point
            gameStatus = LOST;
   } // end while 

   // display won or lost message
   if ( gameStatus == WON )
	  printf("Player wins\n");
   else
	  printf("Player loses\n");

   printf("\n");
   system("pause");
   return 0; // indicates successful termination
} // end main

// roll dice, calculate sum and display results
int rollDice()
{
   // pick random die values
   int die1 = 1 + rand() % 6; // first die roll
   int die2 = 1 + rand() % 6; // second die roll
   
   int sum = die1 + die2; // compute sum of die values

   // display results of this roll
   printf("Player rolled %d + %d = %d\n",die1,die2,sum);

   return sum; // return sum of dice
} // end function rollDice


/**************************************************************************
 * (C) Copyright 1992-2005 by Deitel & Associates, Inc. and               *
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
 **************************************************************************/
