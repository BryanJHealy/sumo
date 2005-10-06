#ifndef HelpPrinter_h
#define HelpPrinter_h
/***************************************************************************
                          HelpPrinter.h
                          A class to print the help
                             -------------------
    begin                : Mon, 15 Apr 2002
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.3.2.1  2004/12/21 09:33:13  dkrajzew
// debugging and version patching
//
// Revision 1.4  2004/12/20 13:15:59  dkrajzew
// options output corrected
//
// Revision 1.3  2004/11/23 10:27:45  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:47:17  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:09:09  dkrajzew
// initial commit for some utility classes common to most propgrams of the sumo-package
//
// Revision 1.6  2002/06/11 14:38:21  dkrajzew
// windows eol removed
//
// Revision 1.5  2002/06/11 13:43:36  dkrajzew
// Windows eol removed
//
// Revision 1.4  2002/06/10 08:33:23  dkrajzew
// Parsing of strings into other data formats generelized; Options now recognize false numeric values; documentation added
//
// Revision 1.3  2002/04/17 11:19:57  dkrajzew
// windows-carriage returns removed
//
// Revision 1.2  2002/04/16 06:52:01  dkrajzew
// documentation added; coding standard attachements added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class HelpPrinter
 * A class that prints a list of strings unless a null-string occures
 * This class was made due to the wish to interrupt the output after a
 * certain number of lines - a feature that is not yet implemented
 */
class HelpPrinter {
public:
    /** prints the given lines into cout */
    static void print(char *help[]) {
        for(size_t i=0; help[i]!=0; i++)
            std::cout << help[i] << std::endl;
    }

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
//
