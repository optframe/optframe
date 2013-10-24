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
	System.out.println("DEPRECATED!!!!!! USE SHELL VERSION...               ");
	System.out.println("OptFrame development version - Project Generator MCT");
	System.out.println("             \"Make a Compilable Thing!\"           ");
	System.out.println("      http://sourceforge.net/projects/optframe/     ");
	System.out.println("====================================================");
	String saida = input.readLine();
    if(saida != "")
        return;
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
	System.out.println();


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


	// ## Creating Solution file

	var_inc="./"+project+"/Solution.h";
	var="./MyProjects/"+project+"/Solution.h";

	if ( copyfile( "./mct/Solution.tpl", var ))
	{ 
	    System.out.println( "2. Creating Solution File...[ok]" );

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$project", project);

	    writeToFile(sVar,var);

	    appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h") ;
	} 
	else 
	{
	    System.out.println( "2. Creating Solution File...[fail]" );
	    return;
	}


	//##############################################
	//#          Memory Structure
	//##############################################

	String memproject;
	String commamproject;
	String typeproject;
	String initializememory;

	System.out.println("What's your Memory Structure? It is used for fast re-evaluation. (if it is not necessary leave this field empty)");
	String mem = input.readLine();

	if ( ! mem.equals("") )
	{
	    mem = mem.replaceAll(">", " > ");
	    mem = mem.replaceAll("<", " < ");

	    System.out.println("Do you need any extra include? (ex.: \"xyz.h\" or <vector>)");
	    include = "";
	    incl = input.readLine();

	    while ( ! incl.equals("") )//
	    {
		include += "\n#include " + incl;
		incl = input.readLine();
	    }

	    copyfile("./mct/MemTest.tpl","./MemTest.cpp");

	    String sMemTest = loadfile("MemTest.cpp");

	    sRepTest = sRepTest.replaceAll("\\$mem", mem);
	    sRepTest = sRepTest.replaceAll("\\$include", include);

	    writeToFile(sRepTest,"RepTest.cpp");

	    System.out.println("Warning: cannot compile the source code");
	    System.out.println();


	    typeproject="typedef "+mem+" Mem"+project+";";
	    memproject="Mem"+project;
	    commamproject=" , Mem"+project;
	    initializememory=" , * new Mem"+project;
	}
	else
	{
	    System.out.println("No memory structure!");
	    typeproject="";
	    memproject="";
	    commamproject="";
	    initializememory=" , * new int";
	}

	System.out.println();


	//## Creating Memory file

	var_inc = "./"+project+"/Memory.h" ;
	var = "./MyProjects/"+project+"/Memory.h";

	if ( copyfile ( "./mct/Memory.tpl", var ) )
	{
	    System.out.println("3. Creating Memory File...[ok]");

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$typeproject", typeproject);
	    sVar = sVar.replaceAll("\\$include", include);
	    sVar = sVar.replaceAll("\\$project", project);

	    writeToFile(sVar,var);

	    appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h");
	} 
	else 
	{ 
	    System.out.println( "3. Creating Memory File...[fail]" );
	    return;
	}


	//## Creating Evaluation file

	var_inc = "./"+project+"/Evaluation.h" ;
	var = "./MyProjects/"+project+"/Evaluation.h";

	if ( copyfile ( "./mct/Evaluation.tpl", var ) )
	{
	    System.out.println("4. Creating Evaluation File...[ok]");

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$memproject", memproject);
	    sVar = sVar.replaceAll("\\$project", project);

	    writeToFile(sVar,var);

	    appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h");
	}
	else 
	{ 
	    System.out.println( "4. Creating Evaluation File...[fail]" );
	    return;
	}

	//##############################################
	//#             Problem Instance
	//##############################################

	var_inc = "./"+project+"/ProblemInstance.hpp" ;
	var = "./MyProjects/"+project+"/ProblemInstance.hpp";

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


	//##############################################
	//#             Evaluator
	//##############################################

	String epsilon;

	System.out.println("\nIs this a MINIMIZATION or MAXIMIZATION problem?");
	String minmax = input.readLine();

	if ( minmax.equals("MINIMIZATION") )
	    epsilon="(e1.evaluation() < (e2.evaluation() - EPSILON_"+project+"));";
	else
	    epsilon="(e1.evaluation() > (e2.evaluation() + EPSILON_"+project+"));";


	var_inc = "./"+project+"/Evaluator.hpp" ;
	var = "./MyProjects/"+project+"/Evaluator.hpp";

	if ( copyfile ( "./mct/Evaluator.tpl", var ) )
	{
	    System.out.println("6. Creating Evaluator...[ok]");

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$epsilon", epsilon);
	    sVar = sVar.replaceAll("\\$project", project);
	    sVar = sVar.replaceAll("\\$initializememory", initializememory);
	    sVar = sVar.replaceAll("\\$minmax", minmax);
	    sVar = sVar.replaceAll("\\$commamproject", commamproject);

	    writeToFile(sVar,var);

	    appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h") ;
	} 
	else 
	{ 
	    System.out.println( "6. Creating Evaluator...[fail]" );
	    return;
	}


	//##############################################
	//#               Neighborhood
	//##############################################

	System.out.println("\nHow many Neighborhood Structures will be there in your project?");
	String str_nbNS = input.readLine();

	Scanner toInt = new Scanner(str_nbNS);
	int nbNS = toInt.nextInt();

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

	    String neighborhood = input.readLine();

	    var_inc = "./"+project+"/NSSeq"+neighborhood+".hpp";
	    var = "./MyProjects/"+project+"/NSSeq"+neighborhood+".hpp";

	    if ( copyfile ( "./mct/NSSeq.tpl", var ) )
	    {
		System.out.println("7."+i+" Creating Neighborhood Structure "+neighborhood+" ...[ok]");

		String sVar = loadfile( var );

		sVar = sVar.replaceAll("\\$project", project);
		sVar = sVar.replaceAll("\\$neighborhood", neighborhood);
		sVar = sVar.replaceAll("\\$commamproject", commamproject);

		writeToFile(sVar,var);

		appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h");
	    } 
	    else 
	    { 
		System.out.println( "7."+i+" Creating Neighborhood Structure "+neighborhood+" ...[fail]" );
		return;
	    }
	}


	//##############################################
	//#             Initial Solution
	//##############################################

	System.out.println("\nHow many Initial Solution Generators will be there in your project?");
	String str_nbISG = input.readLine();

	toInt = new Scanner(str_nbISG);
	int nbISG = toInt.nextInt();

	String initialsolution = "";

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


	    initialsolution = input.readLine();


	    var_inc = "./"+project+"/InitialSolution"+initialsolution+".hpp";
	    var = "./MyProjects/"+project+"/InitialSolution"+initialsolution+".hpp";

	    if ( copyfile ( "./mct/InitialSolution.tpl", var ) )
	    {
		System.out.println("8."+i+" Creating Initial Solution Generator "+initialsolution+" ...[ok]");

		String sVar = loadfile( var );

		sVar = sVar.replaceAll("\\$project", project);
		sVar = sVar.replaceAll("\\$initialsolution", initialsolution);

		writeToFile(sVar,var);

		appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h");
	    } 
	    else 
	    { 
		System.out.println( "8."+i+" Creating Initial Solution Generator "+initialsolution+" ...[fail]" );
		return;
	    }
	}


	//##############################################
	//#               Problem Command
	//##############################################

	var_inc = "./"+project+"/ProblemCommand.hpp" ;
	var = "./MyProjects/"+project+"/ProblemCommand.hpp";

	if ( copyfile ( "./mct/ProblemCommand.tpl", var ) )
	{
	    System.out.println("9. Creating Problem Command...[ok]");

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$project", project);
	    sVar = sVar.replaceAll("\\$commamproject", commamproject);
	    sVar = sVar.replaceAll("\\$initialsolution", initialsolution);

	    writeToFile(sVar,var);

	    appendToFile( "#include \""+var_inc+"\""+"\n" , "./MyProjects/"+project+".h");
	} 
	else 
	{ 
	    System.out.println( "9. Creating Problem Command...[fail]" );
	    return;
	}


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
	    sVar = sVar.replaceAll("\\$commamproject", commamproject);
	    sVar = sVar.replaceAll("\\$initialsolution", initialsolution);
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

	var="./makefile";

	if ( copyfile ( "./mct/makefile.tpl", var ) )
	{
	    System.out.println("makefile...[ok]");

	    String sVar = loadfile( var );

	    sVar = sVar.replaceAll("\\$project", project);

	    writeToFile(sVar,var);
	} 
	else 
	{ 
	    System.out.println( "makefile...[fail]" );
	    return;
	}


	System.out.println();
	System.out.println("Congratulations! You can use the following command to compile your project:");
	System.out.println("g++ ./MyProjects/main"+project+".cpp ./OptFrame/Scanner++/Scanner.cpp -o main"+project);
	System.out.println("or you can simply type: \"make\"");

	System.out.println();
	System.out.println("Goodbye!");

    }

}


