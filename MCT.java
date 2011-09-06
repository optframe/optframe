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
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.Scanner;

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

    public static boolean writeToFile(String s, String filename)
    {
	try
	{
	    FileWriter fstream = new FileWriter(filename, false);
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

    private static boolean copyfile(String srFile, String dtFile){
	try{
	    File f1 = new File(srFile);
	    File f2 = new File(dtFile);
	    InputStream in = new FileInputStream(f1);

	    //For Overwrite the file.
	    OutputStream out = new FileOutputStream(f2);

	    byte[] buf = new byte[1024];
	    int len;
	    while ((len = in.read(buf)) > 0){
		out.write(buf, 0, len);
	    }
	    in.close();
	    out.close();
	    //System.out.println("File copied.");
	    return true;
	}
	catch(FileNotFoundException ex){
	    System.out.println(ex.getMessage() + " in the specified directory.");
	    return false;
	}
	catch(IOException e){
	    System.out.println(e.getMessage()); 
	    return false; 
	}
    }

    public static String loadfile(String filename)
    {
	String s = "";
	Scanner scan;
	try {
	    scan = new Scanner(new File(filename));
	    while (scan.hasNextLine())
		s += scan.nextLine() + "\n";
	} catch (FileNotFoundException e) {
	    e.printStackTrace();
	    return "";
	}
	return s;
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

	// ##############################################
	// #          Solution Representation
	// ##############################################

	System.out.println("What's your Solution Representation?");
	String rep = input.readLine();
	rep = rep.replaceAll(">", " > ");
	rep = rep.replaceAll("<", " < ");

	System.out.println("Do you need any extra include? (ex.: \"xyz.h\" or <vector>)");
	String include = "";
	String incl = input.readLine();

	while ( ! incl.equals("") )//
	{
	    include += "\n#include " + incl;
	    incl = input.readLine();
	}

	copyfile("./mct/RepTest.tpl","./RepTest.cpp");

	String sRepTest = loadfile("RepTest.cpp");

	sRepTest = sRepTest.replaceAll("\\$rep", rep);
	sRepTest = sRepTest.replaceAll("\\$include", include);

	writeToFile(sRepTest,"RepTest.cpp");

	System.out.println("Warning: cannot compile the source code");

	// ## Creating Representation file


	String var_inc = "./"+project+"/Representation.h" ;
	var = "./MyProjects/"+project+"/Representation.h";

	if ( copyfile ( "./mct/Representation.tpl", var ) )
	{
	    System.out.println("1. Creating Representation File...[ok]");

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$rep", rep);
	    sVar = sVar.replaceAll("\\$include", include);
	    sVar = sVar.replaceAll("\\$project", project);

	    writeToFile(sVar,var);

	    appendToFile( "#include \""+var_inc+"\"" , "./MyProjects/"+project+".h") ;

	} else { 
	    System.out.println( "1. Creating Representation File...[fail]" );
	    return;
	}

	// ## Creating Solution file

	var_inc="./"+project+"/Solution.h";
	var="./MyProjects/"+project+"/Solution.h";

	if ( copyfile( "./mct/Solution.tpl", var ))
	{ 
	    System.out.println( "2. Creating Solution File...[ok]" );

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$project", project);

	    writeToFile(sVar,var);

	    appendToFile( "#include \""+var_inc+"\"" , "./MyProjects/"+project+".h") ;
	} else 
	    System.out.println( "2. Creating Solution File...[fail]" );
	return;
    }
}
