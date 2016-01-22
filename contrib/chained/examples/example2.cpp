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

void example2(Config& cfg)
{
  // Get the store name.
  try
  {
    string name = cfg.lookup("name");
    cout << "Store name: " << name << endl << endl;
  }
  catch(const SettingNotFoundException &nfex)
  {
    cerr << "No 'name' setting in configuration file." << endl;
  }
  catch (const SettingTypeException& tex)
  {
	  cerr << "'" << tex.getPath() << "' setting is of wrong type" << endl;
  }

  const Setting& root = cfg.getRoot();

  // Output a list of all books in the inventory.
  try
  {
    const Setting &books = root["inventory"]["books"];
    int count = books.getLength();

    cout << setw(30) << left << "TITLE" << "  "
         << setw(30) << left << "AUTHOR" << "   "
         << setw(6) << left << "PRICE" << "  "
         << "QTY"
         << endl;

    for(int i = 0; i < count; ++i)
    {
      const Setting &book = books[i];

      // Only output the record if all of the expected fields are present.
      string title, author;
      double price;
      int qty;

      if(!(book.lookupValue("title", title)
           && book.lookupValue("author", author)
           && book.lookupValue("price", price)
           && book.lookupValue("qty", qty)))
        continue;

      cout << setw(30) << left << title << "  "
           << setw(30) << left << author << "  "
           << '$' << setw(6) << right << price << "  "
           << qty
           << endl;
    }
    cout << endl;
  }
  catch(const SettingNotFoundException &nfex)
  {
    // Ignore.
  }

  // Output a list of all books in the inventory.
  try
  {
    const Setting &movies = root["inventory"]["movies"];
    int count = movies.getLength();

    cout << setw(30) << left << "TITLE" << "  "
         << setw(10) << left << "MEDIA" << "   "
         << setw(6) << left << "PRICE" << "  "
         << "QTY"
         << endl;

    for(int i = 0; i < count; ++i)
    {
      const Setting &movie = movies[i];

      // Only output the record if all of the expected fields are present.
      string title, media;
      double price;
      int qty;

      if(!(movie.lookupValue("title", title)
           && movie.lookupValue("media", media)
           && movie.lookupValue("price", price)
           && movie.lookupValue("qty", qty)))
        continue;

      cout << setw(30) << left << title << "  "
           << setw(10) << left << media << "  "
           << '$' << setw(6) << right << price << "  "
           << qty
           << endl;
    }
    cout << endl;
  }
  catch(const SettingNotFoundException &nfex)
  {
    // Ignore.
  }
}
