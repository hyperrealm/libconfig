/* ----------------------------------------------------------------------------
   libconfig - A library for processing structured configuration files
   libconfig chained - Extension for reading the configuration and defining 
                       the configuration specification at once.
   Copyright (C) 2016 Richard Schubert

   This file is part of libconfig contributions.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, see
   <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------------
*/

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "../libconfig_chained.h"

using namespace std;
using namespace libconfig;

// This example reads the configuration file 'example.cfg' and displays
// some of its contents.

void example1(Config& cfg)
{
	ChainedSetting cs(cfg.getRoot());

	string name = cs["name"].defaultValue("title");
	double longitude = cs["longitude"].defaultValue(13.41);
	double latitude = cs["latitude"].defaultValue(52.52);

	if (longitude > 0.0)
	{

	}
}
