// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

//#######################################################################
//# OptFrame - Project Generator MCT - "Make a Compilable Thing!"
//# http://optframe.sourceforge.net           
//#######################################################################
//#
//# Use this script to generate a new project (.hpp files and makefile)
//#
//#######################################################################

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;

public class MCT
{
    public static boolean appendToFile(String s, String filename)
    {
	try
	{
	    FileWriter fstream = new FileWriter(filename, true);
	    BufferedWriter out = new BufferedWriter(fstream);
	    out.write(s);
	    out.close();
	}
	catch (Exception e)
	{
	    System.err.println("Error: " + e.getMessage());
	    return false;
	}

	return true;
    }

    public static void main(String[] args) throws IOException
    {
	BufferedReader input = new BufferedReader(new InputStreamReader(System.in));

	System.out.println("====================================================");
	System.out.println("OptFrame development version - Project Generator MCT");
	System.out.println("             \"Make a Compilable Thing!\"           ");
	System.out.println("      http://sourceforge.net/projects/optframe/     ");
	System.out.println("====================================================");
	System.out.println();
	System.out.println("1. What's the name of your project? (Step 1 of 9)");
	String name = input.readLine();
	String project = name;

	project = project.replaceAll(" ", ""); // remove white spaces
	project = project.replaceAll("/", ""); // remove slashes

	System.out.println("\nIs \"" + project + "\" a good abbreviation for your project?\nType the abbreviation or leave it blank.");
	String p2 = input.readLine();

	if (!(p2.compareTo("") == 0))
	    project = p2;

	project = project.replaceAll(" ", ""); // remove white spaces
	project = project.replaceAll("/", ""); // remove slashes

	System.out.println();
	System.out.println("Project file will be named " + project + ".h");
	System.out.println();

	// ##############################################
	// # Creating directory
	// ##############################################

	File dir = new File("./MyProjects/" + project);

	if (dir.mkdirs())
	    System.out.println("Creating project directory...[ok]");
	else
	{
	    System.out.println("Creating project directory...[fail]");
	    return;
	}

	// ##############################################
	// # Creating files
	// ##############################################

	if (appendToFile("", "./MyProjects/" + project + ".h"))
	{
	    System.out.println("Project file: " + project + ".h [ok]");
	}
	else
	{
	    System.out.println("Project file: " + project + ".h [fail]");
	    return;
	}

	System.out.println();
	System.out.println("Creating project \"" + name + "\"");

	String var = "#ifndef " + project + "_H_";
	var = var + "\n";
	var = var + "#define " + project + "_H_";
	var = var + "\n";

	appendToFile(var, "./MyProjects/" + project + ".h");

	System.out.println();
	
	

    }
}
