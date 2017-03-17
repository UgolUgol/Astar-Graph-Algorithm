#include <iostream>
#include "Graph.h"
#include <vector>
#include <ctime>
#include <fstream>
using namespace std;

struct TFiles
{
    const char* Input;
    const char* Output;
    const char* Requests;
};


int main(int argc, char* argv[])
{
	bool  preprocess = (argc == 6 && !(strcmp(argv[1],"preprocess")));
	bool search = (argc >= 5 && !strcmp(argv[1], "search"));
    TFiles filename;
    
	if (preprocess)
	{
	    if(!strcmp(argv[2], "--input") && !strcmp(argv[4], "--output"))
        {
            filename.Input = argv[3];
            filename.Output = argv[5];
        }
        else if(!strcmp(argv[2], "--output") && !strcmp(argv[4], "--input"))
        {
            filename.Input = argv[5];
            filename.Output = argv[3];
        }
        else
        {
            std::cout<<"./prog preprocess --input <input file> --output <preprocessed graph>"<<std::endl;
            return 0;
        }
		TPreprocessGraph s1;
		s1.Preprocess(filename.Input,filename.Output);
	}
	else if (search)
	{
        bool quiteMode = true;

        /*------------------------*/
	    for(int i = 2; i < argc;)
        {
         if(!strcmp(argv[i], "--input"))
         {
             filename.Requests = argv[i+1];
             i += 2;
         }
         else if(!strcmp(argv[i], "--output"))
         {
             filename.Output = argv[i+1];
             i += 2;
         }
         else if(!strcmp(argv[i], "--graph"))
         {
             filename.Input = argv[i+1];
             i += 2;
         }
         else if(!strcmp(argv[i], "--full-output"))
         {
             quiteMode = false;
             ++i;
         }
         else
         {
             cout<<"./prog search --graph <preprocessed graph> --input <input file> --output <output file> [--full-output] "<<endl;
             return 0;
         }
        }
        /*------------------------*/

		ifstream input(filename.Input, std::ios::binary);
		TGraph a;
		input >> a;
		input.close();

		uint64_t q;
		std::ifstream request(filename.Requests, std::ios::binary);
		std::ofstream output(filename.Output, std::ios::binary);
		request.read(UINT(&q), sizeof(uint64_t));

		TEdge<uint64_t, uint64_t>* queries = new TEdge<uint64_t, uint64_t>[q];
		request.read(UINT(queries), q * sizeof(TEdge<uint64_t, uint64_t>));
		for (uint64_t i = 0; i < q; ++i)
		{
			a.FindPath(queries[i].first, queries[i].second, output, quiteMode);
		}
		delete[] queries;
		queries = nullptr;
		request.close();
		output.close();
	}

	return 0;
}
