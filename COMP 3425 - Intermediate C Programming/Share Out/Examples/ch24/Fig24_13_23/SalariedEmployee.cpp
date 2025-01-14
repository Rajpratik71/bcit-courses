// Fig. 24.16: SalariedEmployee.cpp
// SalariedEmployee class member-function definitions.
#include <iostream>
using std::cout;

#include "SalariedEmployee.h" // SalariedEmployee class definition

// constructor 
SalariedEmployee::SalariedEmployee( const string &first, 
   const string &last, const string &ssn, double salary )
   : Employee( first, last, ssn )
{ 
   setWeeklySalary( salary ); 
} // end SalariedEmployee constructor

// set salary
void SalariedEmployee::setWeeklySalary( double salary )
{ 
   weeklySalary = ( salary < 0.0 ) ? 0.0 : salary; 
} // end function setWeeklySalary

// return salary
double SalariedEmployee::getWeeklySalary() const
{
   return weeklySalary;
} // end function getWeeklySalary

// calculate earnings; 
// override pure virtual function earnings in Employee
double SalariedEmployee::earnings() const 
{ 
   return getWeeklySalary(); 
} // end function earnings

// print SalariedEmployee's information 
void SalariedEmployee::print() const
{
   cout << "salaried employee: ";
   Employee::print(); // reuse abstract base-class print function
   cout << "\nweekly salary: " << getWeeklySalary();
} // end function print

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
 **************************************************************************/
