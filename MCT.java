// OptFrame - Optimization Framework

// Copyright (C) 2009-2017
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
//# OptFrame - Project Generator MCT - "Make a Compilable Thing!" (Java)
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

	appendToFile(var+"\n", "./MyProjects/" + project + ".h");

	System.out.println();

	// ##############################################
	// #          Solution Representation
	// ##############################################

	System.out.println("The Representation is the data structure that defines/implements the Solution Space.\nWhat's your Representation?");
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

        try {
          //Runtime r = Runtime.getRuntime();
          Process p = Runtime.getRuntime().exec("g++ RepTest.cpp -o RepTest");
          BufferedReader b = new BufferedReader(new InputStreamReader(p.getInputStream()));
          BufferedReader e = new BufferedReader(new 
     InputStreamReader(p.getErrorStream()));
          String line = "";
          while ((line = b.readLine()) != null)
             System.out.println(line);
          while ((line = e.readLine()) != null)
             System.out.println(line);
          b.close();
          e.close();
          p.waitFor();
          if(p.exitValue() == 0) {
             System.out.println("Solution Representation Test...[ok]");
	     System.out.println();
          }
          else {
	     System.out.println("Solution Representation Test...[fail] error code:" + p.exitValue());
	     System.out.println();
          }
        }
        catch(Throwable t) {
           t.printStackTrace();
           System.out.println("Solution Representation Test...[fail] JAVA EXCEPTION");
        }

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

	    appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h") ;

	} 
	else 
	{ 
	    System.out.println( "1. Creating Representation File...[fail]" );
	    return;
	}


	//##############################################
	//#          Auxiliary Data Structure
	//##############################################

	System.out.println("2-3-4. Delta Structure and Solution files are deprecated, use the Solution ADS instead. Moving to next question...");


	//##############################################
	//#             Problem Instance
	//##############################################

	var_inc = "./"+project+"/ProblemInstance.h" ;
	var = "./MyProjects/"+project+"/ProblemInstance.h";

	if ( copyfile ( "./mct/ProblemInstance.tpl", var ) )
	{
	    System.out.println("5. Creating Problem Instance...[ok]");

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$project", project);

	    writeToFile(sVar,var);

	    appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h");
	} 
	else 
	{ 
	    System.out.println( "5. Creating Problem Instance...[fail]" );
	    return;
	}

	var = "./MyProjects/"+project+"/ProblemInstance.cpp";

	if ( copyfile ( "./mct/ProblemInstanceCpp.tpl", var ) )
	{
	    System.out.println("5. Creating Problem Instance (CPP)...[ok]");

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$project", project);

	    writeToFile(sVar,var);
	} 
	else 
	{ 
	    System.out.println( "5. Creating Problem Instance (CPP)...[fail]" );
	    return;
	}


	//##############################################
	//#             Evaluator
	//##############################################

	String epsilon;

	System.out.println("\nIs this a MINIMIZATION or MAXIMIZATION problem?");
	String minmax = input.readLine();
        String isMin = "";

	if ( minmax.equals("MINIMIZATION") ) {
	    epsilon="(f1 < (f2 - EPSILON_"+project+"));";
            isMin="true;";
        }
	else {
	    epsilon="(f1 > (f2 + EPSILON_"+project+"));";
            isMin="false;";
        }

	var_inc = "./"+project+"/Evaluator.h" ;
	var = "./MyProjects/"+project+"/Evaluator.h";

	if ( copyfile ( "./mct/Evaluator.tpl", var ) )
	{
	    System.out.println("6. Creating Evaluator...[ok]");

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$epsilon", epsilon);
	    sVar = sVar.replaceAll("\\$project", project);
	    sVar = sVar.replaceAll("\\$isMin", isMin);
	    sVar = sVar.replaceAll("\\$minmax", minmax);
	    //sVar = sVar.replaceAll("\\$commamproject", commamproject);

	    writeToFile(sVar,var);

	    appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h") ;
	} 
	else 
	{ 
	    System.out.println( "6. Creating Evaluator...[fail]" );
	    return;
	}

	var = "./MyProjects/"+project+"/Evaluator.cpp";

	if ( copyfile ( "./mct/EvaluatorCpp.tpl", var ) )
	{
	    System.out.println("6. Creating Evaluator (CPP)...[ok]");

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$epsilon", epsilon);
	    sVar = sVar.replaceAll("\\$project", project);
	    sVar = sVar.replaceAll("\\$isMin", isMin);
	    sVar = sVar.replaceAll("\\$minmax", minmax);
	    //sVar = sVar.replaceAll("\\$commamproject", commamproject);

	    writeToFile(sVar,var);

	    appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h") ;
	} 
	else 
	{ 
	    System.out.println( "6. Creating Evaluator (CPP)...[fail]" );
	    return;
	}


	//##############################################
	//#               Neighborhood
	//##############################################

	System.out.println("\nHow many Neighborhood Structures will be there in your project?");
	String str_nbNS = input.readLine();

	Scanner toInt = new Scanner(str_nbNS);
	int nbNS = toInt.nextInt();

        String neighborhood = "";
	for (int i=1; i<=nbNS; i++)
	{
	    if (i==1)
		System.out.println("Type the name of your 1st Neighborhood Structure:");
	    else if(i==2)
		System.out.println("Type the name of your 2nd Neighborhood Structure:");
	    else if(i==3)
		System.out.println("Type the name of your 3rd Neighborhood Structure:");
	    else 
		System.out.println("Type the name of your "+i+"th Neighborhood Structure:");

	    neighborhood = input.readLine();

	    var_inc = "./"+project+"/NSSeq"+neighborhood+".h";
	    var = "./MyProjects/"+project+"/NSSeq"+neighborhood+".h";

	    if ( copyfile ( "./mct/NSSeq.tpl", var ) )
	    {
		System.out.println("7."+i+" Creating Neighborhood Structure "+neighborhood+" ...[ok]");

		String sVar = loadfile( var );

		sVar = sVar.replaceAll("\\$project", project);
		sVar = sVar.replaceAll("\\$neighborhood", neighborhood);
		//sVar = sVar.replaceAll("\\$commamproject", commamproject);

		writeToFile(sVar,var);

		appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h");
	    } 
	    else 
	    { 
		System.out.println( "7."+i+" Creating Neighborhood Structure "+neighborhood+" ...[fail]" );
		return;
	    }

	    var_inc = "./"+project+"/NSSeq"+neighborhood+".cpp";
	    var = "./MyProjects/"+project+"/NSSeq"+neighborhood+".cpp";

	    if ( copyfile ( "./mct/NSSeqCpp.tpl", var ) )
	    {
		System.out.println("7."+i+" Creating Neighborhood Structure "+neighborhood+" (CPP) ...[ok]");

		String sVar = loadfile( var );

		sVar = sVar.replaceAll("\\$project", project);
		sVar = sVar.replaceAll("\\$neighborhood", neighborhood);
		//sVar = sVar.replaceAll("\\$commamproject", commamproject);

		writeToFile(sVar,var);
	    } 
	    else 
	    { 
		System.out.println( "7."+i+" Creating Neighborhood Structure "+neighborhood+" (CPP) ...[fail]" );
		return;
	    }
	}


	//##############################################
	//#             Initial Solution
	//##############################################

	System.out.println("\nHow many Constructive methods will be there in your project?");
	String str_nbISG = input.readLine();

	toInt = new Scanner(str_nbISG);
	int nbISG = toInt.nextInt();

	String constructive = "";

	for (int i=1; i<=nbISG; i++ )
	{
	    if (i==1)
		System.out.println("Type the name of your 1st Initial Solution Generator:");
	    else if(i==2)
		System.out.println("Type the name of your 2nd Initial Solution Generator:");
	    else if (i==3)
		System.out.println("Type the name of your 3rd Initial Solution Generator:");
	    else
		System.out.println("Type the name of your "+i+"th Initial Solution Generator:");


	    constructive = input.readLine();


	    var_inc = "./"+project+"/Constructive"+constructive+".h";
	    var = "./MyProjects/"+project+"/Constructive"+constructive+".h";

	    if ( copyfile ( "./mct/Constructive.tpl", var ) )
	    {
		System.out.println("8."+i+" Creating Constructive "+constructive+" ...[ok]");

		String sVar = loadfile( var );

		sVar = sVar.replaceAll("\\$project", project);
		sVar = sVar.replaceAll("\\$constructive", constructive);

		writeToFile(sVar,var);

		appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h");
	    } 
	    else 
	    { 
		System.out.println( "8."+i+" Creating Constructive "+constructive+" ...[fail]" );
		return;
	    }

	    var = "./MyProjects/"+project+"/Constructive"+constructive+".cpp";

	    if ( copyfile ( "./mct/ConstructiveCpp.tpl", var ) )
	    {
		System.out.println("8."+i+" Creating Constructive "+constructive+" (CPP) ...[ok]");

		String sVar = loadfile( var );

		sVar = sVar.replaceAll("\\$project", project);
		sVar = sVar.replaceAll("\\$constructive", constructive);

		writeToFile(sVar,var);
	    } 
	    else 
	    { 
		System.out.println( "8."+i+" Creating Constructive "+constructive+" (CPP) ...[fail]" );
		return;
	    }
	}

        System.out.println("9. Problem Command is deprecated, ignoring...");

	//# Closing project file

	appendToFile( "#endif /*"+project+"_H_*/"+"\n" , "./MyProjects/"+project+".h");

	//##############################################
	//#             Main file
	//##############################################

	var = "./MyProjects/main"+project+".cpp";

	if ( copyfile ( "./mct/main.tpl", var ) )
	{
	    System.out.println("Main file...[ok]");

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$project", project);
	    sVar = sVar.replaceAll("\\$neighborhood", neighborhood);
	    sVar = sVar.replaceAll("\\$constructive", constructive);
	    sVar = sVar.replaceAll("\\$name", name);

	    writeToFile(sVar,var);
	} 
	else 
	{ 
	    System.out.println( "Main file...[fail]" );
	    return;
	}


	//##############################################
	//#          makefile
	//##############################################

	var="./MyProjects/makefile";

	if ( copyfile ( "./mct/makefile.tpl", var ) )
	{
	    System.out.println("makefile...[ok]");

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$project", project);
	    sVar = sVar.replaceAll("\\$constructive", constructive);
	    sVar = sVar.replaceAll("\\$neighborhood", neighborhood);
            
	    writeToFile(sVar,var);
            appendToFile("GCC_FLAGS=-O3 --std=c++11\n", var);
	} 
	else 
	{ 
	    System.out.println( "makefile...[fail]" );
	    return;
	}


	System.out.println();
        System.out.println("Warning: MCT Java may not properly prepare makefiles for multiple projects. A manual fix may be needed.");
	System.out.println();
	System.out.println("Congratulations! You can use the following command to compile your project:");
	System.out.println("g++ --std=c++11 ./MyProjects/main"+project+".cpp ./MyProjects/"+project+"/ProblemInstance.cpp ./MyProjects/"+project+"/Evaluator.cpp ./MyProjects/"+project+"/Constructive"+constructive+".cpp ./MyProjects/"+project+"/NSSeq"+neighborhood+".cpp ./OptFrame/Scanner++/Scanner.cpp -o MyProjects/app_"+project);
	System.out.println("or you can simply type: \"cd MyProjects && make\"");

	System.out.println();
	System.out.println("Goodbye!");

    }

}


