//
//  regularIO.cpp
//  fileio
//
//  Created by Nikhila Cheepurupalli on 12/6/18.
//  Copyright © 2018 Nikhila Cheepurupalli. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;

int main()
{
    int counter[128];
    ifstream inFile;
    char character;
 
    // starts counting the execution time
    auto start = high_resolution_clock::now();

    // reads the file
    inFile.open("/Users/nikhila/Desktop/CSCE311_Project_3/file.txt");
    if (!inFile)
    {
        cout << "The input file could not be opened."<<endl;
        return 1;
    }
    
    // initializes the counter for each letter
    for (int k = 0; k < 128; k++)
    {
        counter[k] = 0;
    }
    
    // Reads the file, and counts the frequency of each letter
    character = inFile.get();
    while (character != EOF)
    {
        cout << character;
        character = toupper(character);
        counter[character]++;
        character = inFile.get();
    }
    // Prints the frequency of each letter
    cout << endl << "Letter frequencies in this file are as follows." << endl;
    for (char ch = 'A'; ch <= 'Z'; ch++)
    {
        cout << ch << " : " << counter[ch] << endl;
    }
    // stops counting the execution time
    auto stop = high_resolution_clock::now();
    
    //finds the execution time in seconds
    auto duration = duration_cast<seconds>(stop - start);

    // prints out execution time
    cout << "Execution time in seconds: "
    << duration.count() << " seconds" << endl;

    
    return 0;
}

