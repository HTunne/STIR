//
// $Id$: $Date$
//
/*!
  \file 
  \ingroup test 
  \brief tests for the convert_array function

  \author Kris Thielemans
  \author PARAPET project

  \date    $Date$
  \version $Revision$
*/
//  TODO remove FULL stuff

#include <iostream>
#include <cmath>

#include "convert_array.h"
#include "IndexRange3D.h"
#include "CPUTimer.h"
#include "RunTests.h"

START_NAMESPACE_TOMO

//! tests  convert_array functionality
class convert_array_Tests : public RunTests
{
public:
  void run_tests();
};


void
convert_array_Tests::run_tests()
{

  cerr << "Test program for 'convert_array'." << endl 
    << "Everything is fine when there is no output below." << endl;
  
  // 1D
  {
    Array<1,float> tf1(1,20);
    tf1.fill(100.F);
    
    Array<1,short> ti1(1,20);
    ti1.fill(100);
    
    {
      // float -> short with a preferred scale factor
      float scale_factor = float(1);
      Array<1,short> ti2 = convert_array(scale_factor, tf1, NumericInfo<short>());
      
      check(scale_factor == float(1));
      check(ti1 == ti2);
    }
    
    
    {
      // float -> short with automatic scale factor
      float scale_factor = 0;
      Array<1,short> ti2 = convert_array(scale_factor, tf1, NumericInfo<short>());
      
      check(fabs(NumericInfo<short>().max_value()/1.01 / ti2[1] -1) < 1E-4);
      for (int i=1; i<= 20; i++)
	ti2[i] = short( double(ti2[i]) *scale_factor);
      check(ti1 == ti2);
    }
    
    tf1 *= 1E20F;
    {
      // float -> short with a preferred scale factor that needs to be adjusted
      float scale_factor = 1;
      Array<1,short> ti2 = convert_array(scale_factor, tf1, NumericInfo<short>());
      
      check(fabs(NumericInfo<short>().max_value()/1.01 / ti2[1] -1) < 1E-4);
      for (int i=1; i<= 20; i++)
	check(fabs(double(ti2[i]) *scale_factor / tf1[i] - 1) < 1E-4) ;
      
    }
    
    {
      // short -> float with a scale factor = 1
      float scale_factor = 1;
      Array<1,float> tf2 = convert_array(scale_factor, ti1, NumericInfo<float>());
      Array<1,short> ti2(1,20);
      
      
      check(scale_factor == float(1));
      check(tf2[1] == 100.F);
      for (int i=1; i<= 20; i++)
	ti2[i] = short(double(tf2[i]) *scale_factor) ;
      check(ti1 == ti2);
    }
    
    {
      // short -> float with a preferred scale factor = .01
      float scale_factor = .01F;
      Array<1,float> tf2 = convert_array(scale_factor, ti1, NumericInfo<float>());
      Array<1,short> ti2(1,20);
      
      check(scale_factor == float(.01));
      //TODO double->short
      for (int i=1; i<= 20; i++)
	ti2[i] = short(double(tf2[i]) *scale_factor + 0.5) ;
      check(ti1 == ti2);
    }
    
    tf1.fill(-3.2F);
    ti1.fill(-3);
    {
      // positive float -> unsigned short with a preferred scale factor
      float scale_factor = 1;
      Array<1,short> ti2 = convert_array(scale_factor, tf1, NumericInfo<short>());
      
      check(scale_factor == float(1));
      check(ti1 == ti2);
    }
    
    {
      Array<1,unsigned short> ti3(1,20);
      ti3.fill(0);
      
      // negative float -> unsigned short with a preferred scale factor
      float scale_factor = 1;
      Array<1,unsigned short> ti2 =
	convert_array(scale_factor, tf1, NumericInfo<unsigned short>());
      
      check(scale_factor == float(1));
      check(ti3 == ti2);
    }
  }
  //   3D

  {
    CPUTimer timer;
    timer.start();
    
    Array<3,float> tf1(IndexRange3D(1,30,1,182,-2,182));
    tf1.fill(100.F);
    
    Array<3,short> ti1(tf1.get_index_range());
    ti1.fill(100);
    
    {
      // float -> short with a preferred scale factor
      float scale_factor = float(1);
      Array<3,short> ti2 = convert_array(scale_factor, tf1, NumericInfo<short>());
      
      check(scale_factor == float(1));
      check(ti1 == ti2);
    }
    
#ifdef FULL    
    {
      // float -> short with automatic scale factor
      float scale_factor = 0;
      Array<3,short> ti2 = convert_array(scale_factor, tf1, NumericInfo<short>());
      
      check(fabs(NumericInfo<short>().max_value()/1.01 / (*ti2.begin_all()) -1) < 1E-4);
      for (Array<3,short>::full_iterator iter= ti2.begin_all();
           iter != ti2.end_all();
	   iter++)
	*iter = short( double((*iter)) *scale_factor);
      check(ti1 == ti2);
    }
    
    tf1 *= 1E20F;
    {
      // float -> short with a preferred scale factor that needs to be adjusted
      float scale_factor = 1;
      Array<3,short> ti2 = convert_array(scale_factor, tf1, NumericInfo<short>());

      check(fabs(NumericInfo<short>().max_value()/1.01 / (*ti2.begin_all()) -1) < 1E-4);
      Array<3,short>::full_iterator iter_ti2= ti2.begin_all();
      Array<3,float>::full_iterator iter_tf1= tf1.begin_all();
      for (;
           iter_ti2 != ti2.end_all();
	   iter_ti2++, iter_tf1++)
	check(fabs(double(*iter_ti2) *scale_factor / *iter_tf1 - 1) < 1E-4) ;
      
    }

    timer.stop();
    cerr << timer.value() << "secs" << endl;
#endif // FULL
  }
#ifdef FULL
  //   3D
  cerr << "org" << endl;

  {
    CPUTimer timer;
    timer.start();
    
    Array<3,float> tf1(IndexRange3D(1,30,1,182,-2,182));
    tf1.fill(100.F);
    
    Array<3,short> ti1(tf1.get_index_range());
    ti1.fill(100);
    
    {
      // float -> short with a preferred scale factor
      float scale_factor = float(1);
      Array<3,short> ti2 = convert_array_org(scale_factor, tf1, NumericInfo<short>());
      
      check(scale_factor == float(1));
      check(ti1 == ti2);
    }
    
    
    {
      // float -> short with automatic scale factor
      float scale_factor = 0;
      Array<3,short> ti2 = convert_array_org(scale_factor, tf1, NumericInfo<short>());
      
      check(fabs(NumericInfo<short>().max_value()/1.01 / (*ti2.begin_all()) -1) < 1E-4);
      for (Array<3,short>::full_iterator iter= ti2.begin_all();
           iter != ti2.end_all();
	   iter++)
	*iter = short( double((*iter)) *scale_factor);
      check(ti1 == ti2);
    }
    
    tf1 *= 1E20F;
    {
      // float -> short with a preferred scale factor that needs to be adjusted
      float scale_factor = 1;
      Array<3,short> ti2 = convert_array_org(scale_factor, tf1, NumericInfo<short>());

      check(fabs(NumericInfo<short>().max_value()/1.01 / (*ti2.begin_all()) -1) < 1E-4);
      Array<3,short>::full_iterator iter_ti2= ti2.begin_all();
      Array<3,float>::full_iterator iter_tf1= tf1.begin_all();
      for (;
           iter_ti2 != ti2.end_all();
	   iter_ti2++, iter_tf1++)
	check(fabs(double(*iter_ti2) *scale_factor / *iter_tf1 - 1) < 1E-4) ;
      
    }
    timer.stop();
    cerr << timer.value() << "secs" << endl;
  }

#endif  
}

END_NAMESPACE_TOMO



USING_NAMESPACE_TOMO



int main()
{
  convert_array_Tests tests;
  tests.run_tests();
  return tests.main_return_value();
}
