//------------------------------------------------------------------------------
// main.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include "core/Math_Library.h"

//------------------------------------------------------------------------------
/**
*/
int
main(int argc, const char** argv)
{
	Example::ExampleApp app;
	if (app.Open())
	{

		app.Run();
		app.Close();
	}
	app.Exit();
	
}