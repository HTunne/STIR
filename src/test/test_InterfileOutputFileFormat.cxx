//
// $Id$
//
/*!

  \file
  \ingroup test

  \brief A simple programme to test the InterfileOutputFileFormat function.

  \author Kris Thielemans

  $Date$
  $Revision$

  
  To run the test, you should use a command line argument with the name of a file.
  This should contain a test par file.
  See InterfileOutputFileFormatTests for file contents.
*/
/*
    Copyright (C) 2002- $Date$, IRSL
    See STIR/LICENSE.txt for details
*/
  
#include "stir/IO/InterfileOutputFileFormat.h"
#include "stir/RunTests.h"
#include "stir/KeyParser.h"
#include "stir/is_null_ptr.h"
#include "stir/Succeeded.h"

#include "stir/VoxelsOnCartesianGrid.h"
#include "stir/CartesianCoordinate3D.h"
#include "stir/IndexRange3D.h"

#include <fstream>
#include <iostream>

#ifndef STIR_NO_NAMESPACES
using std::cerr;
using std::endl;
using std::ifstream;
using std::istream;
#endif

START_NAMESPACE_STIR

/*!
  \ingroup test
  \brief A simple class to test the InterfileOutputFileFormat function.

  The class reads input from a stream, whose contents should be as
  follows:

  \verbatim
  Test InterfileOutputFileFormat Parameters:=
  output file format type := Interfile
  ; here are parameters specific for the file format
  End:=
  \endverbatim

*/
class InterfileOutputFileFormatTests : public RunTests
{
public:
  InterfileOutputFileFormatTests(istream& in) ;

  void run_tests();
private:
  istream& in;
  shared_ptr<OutputFileFormat> output_file_format_ptr;
  KeyParser parser;
};

InterfileOutputFileFormatTests::
InterfileOutputFileFormatTests(istream& in) :
  in(in)
{
  output_file_format_ptr = 0;
  parser.add_start_key("Test InterfileOutputFileFormat Parameters");
  parser.add_parsing_key("output file format type", &output_file_format_ptr);
  parser.add_stop_key("END");
}

void InterfileOutputFileFormatTests::run_tests()
{  
  cerr << "Testing InterfileOutputFileFormat parsing function..." << endl;

  if (!check(parser.parse(in), "parsing failed"))
    return;

  if (!check(!is_null_ptr(output_file_format_ptr), 
        "parsing failed to set output_file_format_ptr"))
    return;
 
  if (!check(dynamic_cast<InterfileOutputFileFormat const *>
          (&(*output_file_format_ptr)) != 0,
        "parsing failed to set output_file_format_ptr to InterfileOutputFileFormat"))
    return;

  cerr << "Output parameters as read from input file:\n"
       << "-------------------------------------------\n";
  cerr << static_cast<ParsingObject&>(*output_file_format_ptr).parameter_info();

  cerr << "-------------------------------------------\n\n";

  cerr << "Now writing to file and reading it back." << endl; 
  // construct density and write to file
  {
    CartesianCoordinate3D<float> origin (0,0,0);  // TODO origin shift currently not supported by Interfile IO
    CartesianCoordinate3D<float> grid_spacing (3,4,5); 
  
    IndexRange<3> 
      range(CartesianCoordinate3D<int>(0,-15,-14),
	    CartesianCoordinate3D<int>(4,14,14));
	    
    VoxelsOnCartesianGrid<float>  image(range,origin, grid_spacing);
    {
      // fill with some data
      float data = .9;
      for (VoxelsOnCartesianGrid<float>::full_iterator iter = image.begin_all();
	   iter != image.end_all();
	 ++iter)
	{
	  *iter = data;
	  data = data*(data+2);
	}
    }

    // write to file

    const Succeeded success =
      output_file_format_ptr->write_to_file("test.hv",image);
    
    if (!check( success==Succeeded::yes, "test writing to file"))
      return;

    // now read it back
    
    shared_ptr<DiscretisedDensity<3,float> >
      density_ptr = DiscretisedDensity<3,float>::read_from_file("test.hv");

    const  VoxelsOnCartesianGrid<float> * image_as_read_ptr =
      dynamic_cast< VoxelsOnCartesianGrid<float> const *>
      (density_ptr.get());
    if (!check(!is_null_ptr(image_as_read_ptr), "test on image type read back from file"))
      return;
    check_if_equal(image_as_read_ptr->get_grid_spacing(), grid_spacing, "test on grid spacing read back from file");


    check_if_equal(image, *density_ptr, "test on data read back from file");
    check_if_equal(density_ptr->get_origin(), origin, "test on origin read back from file");


  }
    


}


END_NAMESPACE_STIR

USING_NAMESPACE_STIR

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    cerr << "Usage : " << argv[0] << " filename\n"
         << "See source file for the format of this file.\n\n";
    return EXIT_FAILURE;
  }


  ifstream in(argv[1]);
  if (!in)
  {
    cerr << argv[0] 
         << ": Error opening input file " << argv[1] << "\nExiting.\n";

    return EXIT_FAILURE;
  }

  InterfileOutputFileFormatTests tests(in);
  tests.run_tests();
  return tests.main_return_value();
}
