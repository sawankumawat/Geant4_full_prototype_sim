#include <iostream>
#include <vector>
#include <string>
#include "TSystemFile.h"
#include "TSystemDirectory.h"
#include "TCollection.h"

void PathFinder(std::string dirname, std::string head, std::string ext, std::vector<std::string> &paths) {
    std::cout << dirname <<std::endl;
    dirname+="/";
    TSystemDirectory dir(dirname.c_str(), dirname.c_str()); 
    TList *files = dir.GetListOfFiles(); 
    if (files) { 
        TSystemFile *file; 
        TString fname; 
        TIter next((TCollection*) files); 
        while ((file=(TSystemFile*)next())) { 
            fname = file->GetName(); 
            if (!file->IsDirectory() && fname.EndsWith(ext.c_str()) && fname.BeginsWith(head.c_str())) { 
                std::cout <<"\t\t"<< dirname<<fname.Data() << std::endl; 
                paths.push_back(dirname+fname.Data());
            }
            else if(file->IsDirectory() && fname!="." && fname!=".."){
                std::cout<<"\t"<<fname.Data()<<std::endl;
                PathFinder(dirname+fname.Data(), head, ext, paths);
            } 
        } 
    } 
    //return paths;
}


